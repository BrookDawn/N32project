# Serial port test script - Modbus slave simulator
# For testing USART2 DMA functionality

Add-Type -AssemblyName System.IO.Ports

function Calculate-CRC16 {
    param([byte[]]$data)

    [uint16]$crc = 0xFFFF
    foreach ($byte in $data) {
        $crc = $crc -bxor $byte
        for ($i = 0; $i -lt 8; $i++) {
            if (($crc -band 0x0001) -ne 0) {
                $crc = ($crc -shr 1) -bxor 0xA001
            } else {
                $crc = $crc -shr 1
            }
        }
    }
    return $crc
}

function Create-ReadResponse {
    param([byte]$slaveAddr, [byte]$funcCode, [byte[]]$regData)

    $response = New-Object System.Collections.ArrayList
    [void]$response.Add($slaveAddr)
    [void]$response.Add($funcCode)
    [void]$response.Add([byte]$regData.Length)
    foreach ($b in $regData) { [void]$response.Add($b) }

    $crc = Calculate-CRC16 -data $response.ToArray()
    [void]$response.Add([byte]($crc -band 0xFF))
    [void]$response.Add([byte](($crc -shr 8) -band 0xFF))

    return [byte[]]$response.ToArray()
}

function Create-WriteSingleResponse {
    param([byte]$slaveAddr, [uint16]$address, [uint16]$value)

    $response = New-Object System.Collections.ArrayList
    [void]$response.Add($slaveAddr)
    [void]$response.Add([byte]0x06)
    [void]$response.Add([byte](($address -shr 8) -band 0xFF))
    [void]$response.Add([byte]($address -band 0xFF))
    [void]$response.Add([byte](($value -shr 8) -band 0xFF))
    [void]$response.Add([byte]($value -band 0xFF))

    $crc = Calculate-CRC16 -data $response.ToArray()
    [void]$response.Add([byte]($crc -band 0xFF))
    [void]$response.Add([byte](($crc -shr 8) -band 0xFF))

    return [byte[]]$response.ToArray()
}

function Create-WriteMultipleResponse {
    param([byte]$slaveAddr, [uint16]$address, [uint16]$count)

    $response = New-Object System.Collections.ArrayList
    [void]$response.Add($slaveAddr)
    [void]$response.Add([byte]0x10)
    [void]$response.Add([byte](($address -shr 8) -band 0xFF))
    [void]$response.Add([byte]($address -band 0xFF))
    [void]$response.Add([byte](($count -shr 8) -band 0xFF))
    [void]$response.Add([byte]($count -band 0xFF))

    $crc = Calculate-CRC16 -data $response.ToArray()
    [void]$response.Add([byte]($crc -band 0xFF))
    [void]$response.Add([byte](($crc -shr 8) -band 0xFF))

    return [byte[]]$response.ToArray()
}

# Main program
$portName = "COM3"
$baudRate = 115200

Write-Host "Opening serial port $portName @ $baudRate bps"

try {
    $port = New-Object System.IO.Ports.SerialPort $portName, $baudRate, "None", 8, "One"
    $port.ReadTimeout = 500
    $port.Open()
    Write-Host "Serial port opened: $($port.PortName)"
} catch {
    Write-Host "Cannot open serial port: $_"
    exit
}

Write-Host "Waiting for Modbus master requests..."
Write-Host "Press Ctrl+C to exit"
Write-Host ""

$requestCount = 0
$buffer = New-Object byte[] 256

try {
    while ($true) {
        if ($port.BytesToRead -gt 0) {
            Start-Sleep -Milliseconds 50
            $count = $port.Read($buffer, 0, $port.BytesToRead)

            if ($count -ge 4) {
                $requestCount++
                $data = $buffer[0..($count-1)]
                $hexStr = ($data | ForEach-Object { $_.ToString("X2") }) -join " "
                Write-Host "[$requestCount] Received $count bytes: $hexStr"

                $slaveAddr = $data[0]
                $funcCode = $data[1]

                if ($funcCode -eq 0x03) {
                    $startAddr = ($data[2] -shl 8) -bor $data[3]
                    $quantity = ($data[4] -shl 8) -bor $data[5]
                    Write-Host "    -> Read request: start=0x$($startAddr.ToString('X4')), qty=$quantity"

                    $regData = New-Object System.Collections.ArrayList
                    for ($i = 0; $i -lt $quantity; $i++) {
                        $val = ($startAddr + $i) -band 0xFFFF
                        [void]$regData.Add([byte](($val -shr 8) -band 0xFF))
                        [void]$regData.Add([byte]($val -band 0xFF))
                    }

                    $response = Create-ReadResponse -slaveAddr $slaveAddr -funcCode $funcCode -regData $regData.ToArray()
                    $port.Write($response, 0, $response.Length)
                    $hexResp = ($response | ForEach-Object { $_.ToString("X2") }) -join " "
                    Write-Host "    <- Response $($response.Length) bytes: $hexResp"

                } elseif ($funcCode -eq 0x06) {
                    $address = ($data[2] -shl 8) -bor $data[3]
                    $value = ($data[4] -shl 8) -bor $data[5]
                    Write-Host "    -> Write single: addr=0x$($address.ToString('X4')), val=0x$($value.ToString('X4'))"

                    $response = Create-WriteSingleResponse -slaveAddr $slaveAddr -address $address -value $value
                    $port.Write($response, 0, $response.Length)
                    $hexResp = ($response | ForEach-Object { $_.ToString("X2") }) -join " "
                    Write-Host "    <- Response $($response.Length) bytes: $hexResp"

                } elseif ($funcCode -eq 0x10) {
                    $address = ($data[2] -shl 8) -bor $data[3]
                    $regCount = ($data[4] -shl 8) -bor $data[5]
                    Write-Host "    -> Write multiple: addr=0x$($address.ToString('X4')), count=$regCount"

                    $response = Create-WriteMultipleResponse -slaveAddr $slaveAddr -address $address -count $regCount
                    $port.Write($response, 0, $response.Length)
                    $hexResp = ($response | ForEach-Object { $_.ToString("X2") }) -join " "
                    Write-Host "    <- Response $($response.Length) bytes: $hexResp"

                } else {
                    Write-Host "    Unknown function code: 0x$($funcCode.ToString('X2'))"
                }

                Write-Host ""
            }
        }

        Start-Sleep -Milliseconds 10
    }
} finally {
    $port.Close()
    Write-Host "Serial port closed"
}
