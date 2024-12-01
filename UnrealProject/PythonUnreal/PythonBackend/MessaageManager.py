import socket
import struct

from .....blivedm import blivedm
import blivedm.models.web as web_models

# 假设您已经有了以下函数来创建socket
def create_socket():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return sock

# 创建socket实例
sock = create_socket()
server_address = ('127.0.0.1', 5555)

class UnrealHandler(blivedm.BaseHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.sock = sock
        self.server_address = server_address

    def _send_data(self, data):
        # 将数据打包成字节流
        packed_data = struct.pack('!I', len(data)) + data.encode('utf-8')
        self.sock.sendto(packed_data, self.server_address)

    def _on_heartbeat(self, client: blivedm.BLiveClient, message: web_models.HeartbeatMessage):
        print(f'[{client.room_id}] 心跳')
        self._send_data(f'heartbeat {client.room_id}')

    def _on_danmaku(self, client: blivedm.BLiveClient, message: web_models.DanmakuMessage):
        print(f'[{client.room_id}] {message.uname}：{message.msg}')
        self._send_data(f'danmaku {client.room_id} {message.uname} {message.msg}')

    def _on_gift(self, client: blivedm.BLiveClient, message: web_models.GiftMessage):
        print(f'[{client.room_id}] {message.uname} 赠送{message.gift_name}x{message.num}'
              f' （{message.coin_type}瓜子x{message.total_coin}）')
        self._send_data(f'gift {client.room_id} {message.uname} {message.gift_name} {message.num} {message.coin_type} {message.total_coin}')

    def _on_buy_guard(self, client: blivedm.BLiveClient, message: web_models.GuardBuyMessage):
        print(f'[{client.room_id}] {message.username} 购买{message.gift_name}')
        self._send_data(f'buy_guard {client.room_id} {message.username} {message.gift_name}')

    def _on_super_chat(self, client: blivedm.BLiveClient, message: web_models.SuperChatMessage):
        print(f'[{client.room_id}] 醒目留言 ¥{message.price} {message.uname}：{message.message}')
        self._send_data(f'super_chat {client.room_id} {message.price} {message.uname} {message.message}')
