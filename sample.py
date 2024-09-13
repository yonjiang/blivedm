# -*- coding: utf-8 -*-
from datetime import datetime
import asyncio
import http.cookies
import random
from typing import *
import websockets

import aiohttp

import blivedm
import blivedm.models.web as web_models


# 直播间ID的取值看直播间URL
TEST_ROOM_IDS = [

    7023230,
]

# 这里填一个已登录账号的cookie的SESSDATA字段的值。不填也可以连接，但是收到弹幕的用户名会打码，UID会变成0
SESSDATA = ''

session: Optional[aiohttp.ClientSession] = None


# 获取程序启动时的日期和时间
start_time = datetime.now()

# 将日期和时间格式化为字符串
formatted_time = start_time.strftime('%Y%m%d%H%M%S')

# 在文件名中包含格式化后的日期和时间
file_name = f'danmaku_{formatted_time}.txt'


async def main():
    init_session()

    # 运行blivedm客户端
    blivedm_task = asyncio.create_task(run_single_client())
    blivedm_task_multi = asyncio.create_task(run_multi_clients())

    # 运行WebSocket服务器
    websocket_task = asyncio.create_task(websocket_server())

    try:
        # 等待任务完成
        await asyncio.gather(blivedm_task, websocket_task, blivedm_task_multi)
    finally:
        await session.close()


def init_session():
    cookies = http.cookies.SimpleCookie()
    cookies['SESSDATA'] = SESSDATA
    cookies['SESSDATA']['domain'] = 'bilibili.com'

    global session
    session = aiohttp.ClientSession()
    session.cookie_jar.update_cookies(cookies)


async def run_single_client():
    """
    演示监听一个直播间
    """
    room_id = random.choice(TEST_ROOM_IDS)
    client = blivedm.BLiveClient(room_id, session=session)
    handler = MyHandler()
    client.set_handler(handler)

    client.start()
    try:
        # 演示5秒后停止
        await asyncio.sleep(5)
        client.stop()

        await client.join()
    finally:
        await client.stop_and_close()


async def run_multi_clients():
    """
    演示同时监听多个直播间
    """
    clients = [blivedm.BLiveClient(room_id, session=session) for room_id in TEST_ROOM_IDS]
    handler = MyHandler()
    for client in clients:
        client.set_handler(handler)
        client.start()

    try:
        await asyncio.gather(*(
            client.join() for client in clients
        ))
    finally:
        await asyncio.gather(*(
            client.stop_and_close() for client in clients
        ))


class MyHandler(blivedm.BaseHandler):
    # # 演示如何添加自定义回调
    # _CMD_CALLBACK_DICT = blivedm.BaseHandler._CMD_CALLBACK_DICT.copy()
    #
    # # 入场消息回调
    # def __interact_word_callback(self, client: blivedm.BLiveClient, command: dict):
    #     print(f"[{client.room_id}] INTERACT_WORD: self_type={type(self).__name__}, room_id={client.room_id},"
    #           f" uname={command['data']['uname']}")
    # _CMD_CALLBACK_DICT['INTERACT_WORD'] = __interact_word_callback  # noqa

    def _on_heartbeat(self, client: blivedm.BLiveClient, message: web_models.HeartbeatMessage):
        print(f'[{client.room_id}] 心跳')

    def _on_danmaku(self, client: blivedm.BLiveClient, message: web_models.DanmakuMessage):
        print(f'[{client.room_id}] {message.uname}：{message.msg}')

        # 发送数据到WebSocket客户端
        asyncio.create_task(send_to_clients({
            'type': 'danmaku',
            'room_id': client.room_id,
            'uname': message.uname,
            'msg': message.msg
        }))

    def _on_gift(self, client: blivedm.BLiveClient, message: web_models.GiftMessage):
        print(f'\033[93m礼物： \033[0m')
        print(f'[{client.room_id}] {message.uname} 赠送{message.gift_name}x{message.num}'
              f' （{message.coin_type}瓜子x{message.total_coin}）')

    def _on_buy_guard(self, client: blivedm.BLiveClient, message: web_models.GuardBuyMessage):
        print(f'\033[95m购买： \033[0m')
        print(f'[{client.room_id}] {message.username} 购买{message.gift_name}')

    def _on_super_chat(self, client: blivedm.BLiveClient, message: web_models.SuperChatMessage):
        print(f'[{client.room_id}] 醒目留言 ¥{message.price} {message.uname}：{message.message}')


# WebSocket 服务器
connected_clients = set()


async def websocket_handler(websocket, path):
    connected_clients.add(websocket)
    try:
        async for message in websocket:
            pass
    finally:
        connected_clients.remove(websocket)


async def send_to_clients(data):
    if connected_clients:
        message = json.dumps(data)
        await asyncio.wait([client.send(message) for client in connected_clients])


async def websocket_server():
    async with websockets.serve(websocket_handler, "localhost", 8765):
        await asyncio.Future()  # run forever

if __name__ == '__main__':
    asyncio.run(main())
