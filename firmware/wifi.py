import socket

UDP_PORT = 5006

# Crear socket UDP
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Permite reutilizar puerto (evita errores raros)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Escuchar en TODAS las interfaces
sock.bind(("0.0.0.0", UDP_PORT))

print("===================================")
print("  ESCUCHANDO UDP - PUERTO 5006")
print("===================================")
print("IP local del PC: escucha en todas")
print("Esperando datos del ESP32...\n")

while True:
    try:
        data, addr = sock.recvfrom(1024)

        mensaje = data.decode(errors="ignore")

        print("-----------------------------------")
        print("Desde:", addr)
        print("Datos:", mensaje)
        print("-----------------------------------\n")

    except Exception as e:
        print("Error recibiendo datos:", e)