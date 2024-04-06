import pygame
import random
from datetime import datetime, timedelta

# 初始化pygame
pygame.init()
# 设置窗口大小和标题
screen_size = (720, 720)
screen = pygame.display.set_mode(screen_size)
pygame.display.set_caption("Temple Incense")
background = pygame.image.load('./incense/bg.png')
background = pygame.transform.scale(background, screen_size)
# 香的颜色和尺寸
INCENSE_COLOR = (139, 69, 19)
INCENSE_WIDTH = 5
INCENSE_HEIGHT = 70
FLAME_COLOR = (255, 255, 0)
FLAME_SIZE = 5
# 字体设置
font_name = pygame.font.match_font('fangsong')
font = pygame.font.Font(font_name, 18)

# 存储香的信息
incenses = []
# 在随机位置添加香
def add_incense(name):
    x = random.randint(0, screen.get_width() - INCENSE_WIDTH)
    y = random.randint(0, screen.get_height() - INCENSE_HEIGHT)
    end_time = datetime.now() + timedelta(minutes=5)
    incenses.append((x, y, end_time, name))
    
def run_pygame():
    # 主循环
    running = True
    last_incense_time = datetime.now()
    while running:
        # 处理事件
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        # # 每秒添加一根香
        # if datetime.now() - last_incense_time >= timedelta(seconds=1):
        #     add_incense("Name")
        #     last_incense_time = datetime.now()

        # 清除屏幕
        screen.blit(background,(0,0))

        # 绘制香
        now = datetime.now()
        incenses[:] = [(x, y, end_time, name) for x, y, end_time, name in incenses if end_time > now]
        for x, y, end_time, name in incenses:
            remaining_ratio = (end_time - now) / timedelta(minutes=5)
            current_height = int(INCENSE_HEIGHT * remaining_ratio)
            pygame.draw.rect(screen, INCENSE_COLOR, (x, y + INCENSE_HEIGHT - current_height, INCENSE_WIDTH, current_height))
            pygame.draw.rect(screen, FLAME_COLOR, (x, y + INCENSE_HEIGHT - current_height - FLAME_SIZE, INCENSE_WIDTH, FLAME_SIZE))
            try:
                text_surface = font.render(name, True, (0, 0, 0))
            except pygame.error:
                text_surface = font.render('?', True, (0, 0, 0))  # 使用'?'代替不认识的字符
            screen.blit(text_surface, (x, y + INCENSE_HEIGHT + 5))

        # 更新屏幕
        pygame.display.flip()

    # 退出pygame
    pygame.quit()
    