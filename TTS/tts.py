import os
from pydub import AudioSegment

def convert_m4a_to_wav(m4a_file, wav_file):
    # 加载 M4A 文件
    audio = AudioSegment.from_file(m4a_file, format='m4a')
    # 导出为 WAV 格式
    audio.export(wav_file, format='wav')

if __name__ == "__main__":
    # 使用相对路径
    script_dir = os.path.dirname(os.path.abspath(__file__))
    m4a_file = os.path.join(script_dir, 'input.m4a')  # 替换为你的 M4A 文件路径
    wav_file = os.path.join(script_dir, 'output.wav')  # 替换为你想要保存的 WAV 文件路径
    
    convert_m4a_to_wav(m4a_file, wav_file)
    print("转换完成！")