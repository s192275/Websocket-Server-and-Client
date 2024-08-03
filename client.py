import websockets
import time
import json
import asyncio

msg_dict = {
    "msg": [],
    "seconds": [],
    "num_of_pack": []
}

print("Server bağlantısı başlatılıyor...")
index = 0

async def receive_data():
    async with websockets.connect("ws://localhost:8080") as websocket:
        for i in range(100000):
            start = time.time()
            await websocket.send("Clienttan selamlar...")
            message = await websocket.recv()
            elapsed = time.time() - start
            print(f"Server mesajı {elapsed:.4f} saniyede geldi. İçerik: {message}")
            msg_dict["msg"].append(message)
            msg_dict["seconds"].append(elapsed)
            msg_dict["num_of_pack"].append(i)

        max_val = max(msg_dict["seconds"])
        with open('logs_websocket.json', 'w') as json_file:
            json.dump(msg_dict, json_file, indent=4)

        print("********** Rapor **********")
        print(f"Ortalama Bekleme Süresi: {sum(msg_dict['seconds']) / len(msg_dict['seconds']):.4f} saniye")
        print(f"Maksimum Geçen Süre: {max(msg_dict['seconds']):.4f} saniye")

        for i in range(len(msg_dict["seconds"])):
            if msg_dict["seconds"][i] == max_val:
                index = i

        print(f"Maksimum Geçen Süreye Ait Paket: {index}")
        print(f"Toplam Geçen Süre: {sum(msg_dict['seconds']):.4f} saniye")

asyncio.run(receive_data())
