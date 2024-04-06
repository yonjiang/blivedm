# -*- coding: utf-8 -*-
import asyncio
import http.cookies
import random
from typing import *


import aiohttp

import blivedm
import blivedm.models.web as web_models

from incense.incense import *

# 直播间ID的取值看直播间URL
TEST_ROOM_IDS = [
    # 12235923,
    # 14327465,
    # 1017,
    # 5050,
    7023230,
    # 21396545,
    # 21449083,
    # 23105590,
]

# 这里填一个已登录账号的cookie的SESSDATA字段的值。不填也可以连接，但是收到弹幕的用户名会打码，UID会变成0
SESSDATA = ''

session: Optional[aiohttp.ClientSession] = None

from datetime import datetime

# 获取程序启动时的日期和时间
start_time = datetime.now()

# 将日期和时间格式化为字符串
formatted_time = start_time.strftime('%Y%m%d%H%M%S')

# 在文件名中包含格式化后的日期和时间
file_name = f'danmaku_{formatted_time}.txt'


# async def main():
#     init_session()
#     # 运行pygame
#     loop = asyncio.get_event_loop()
#     pygame_task = loop.run_in_executor(None, run_pygame)
    
#     try:
#         await run_single_client()
#         await run_multi_clients()
#     finally:
#         await session.close()

async def main():
    init_session()

    # 运行blivedm客户端
    blivedm_task = asyncio.create_task(run_single_client())
    blivedm_task_multi = asyncio.create_task(run_multi_clients())

    # 运行pygame
    pygame_task = asyncio.to_thread(run_pygame)

    try:
        # 等待任务完成
        await asyncio.gather(blivedm_task, pygame_task, blivedm_task_multi)
    finally:
        # 确保所有blivedm任务完成
        await blivedm_task
        await blivedm_task_multi
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
        # if "求好运" in message.msg:
        add_incense(message.uname)
        # 将评论写入文件
        with open(file_name, 'a', encoding='utf-8') as f:
            f.write(f'[{client.room_id}] {message.uname}：{message.msg}\n')

    def _on_gift(self, client: blivedm.BLiveClient, message: web_models.GiftMessage):
        print(f'\033[93m礼物： \033[0m')
        print(f'[{client.room_id}] {message.uname} 赠送{message.gift_name}x{message.num}'
              f' （{message.coin_type}瓜子x{message.total_coin}）')

    def _on_buy_guard(self, client: blivedm.BLiveClient, message: web_models.GuardBuyMessage):
        print(f'\033[95m购买： \033[0m')
        print(f'[{client.room_id}] {message.username} 购买{message.gift_name}')

    def _on_super_chat(self, client: blivedm.BLiveClient, message: web_models.SuperChatMessage):
        print(f'[{client.room_id}] 醒目留言 ¥{message.price} {message.uname}：{message.message}')


if __name__ == '__main__':
    asyncio.run(main())
