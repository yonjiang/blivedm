import tkinter as tk
import pyaudio
import wave
import speech_recognition as sr

class VoiceRecorder:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("麦克风对话系统")

        self.recording = False
        self.frames = []

        self.recognizer = sr.Recognizer()

        self.record_button = tk.Button(self.root, text="按住说话", width=20, height=2)
        self.record_button.pack(pady=20)
        self.record_button.bind('<ButtonPress>', self.start_recording)
        self.record_button.bind('<ButtonRelease>', self.stop_recording)

        self.text_box = tk.Text(self.root, width=50, height=10)
        self.text_box.pack(pady=20)

        self.p = pyaudio.PyAudio()
        self.stream = None

        self.root.mainloop()

    def start_recording(self, event):
        self.recording = True
        self.frames = []

        self.stream = self.p.open(format=pyaudio.paInt16,
                                  channels=1,
                                  rate=44100,
                                  input=True,
                                  frames_per_buffer=1024)

        self.root.after(100, self.record)

    def record(self):
        if self.recording:
            data = self.stream.read(1024)
            self.frames.append(data)
            self.root.after(100, self.record)

    def stop_recording(self, event):
        if self.recording:
            self.recording = False
            self.stream.stop_stream()
            self.stream.close()

            self.save_and_recognize()

    def save_and_recognize(self):
        wf = wave.open("output.wav", 'wb')
        wf.setnchannels(1)
        wf.setsampwidth(self.p.get_sample_size(pyaudio.paInt16))
        wf.setframerate(44100)
        wf.writeframes(b''.join(self.frames))
        wf.close()

        with sr.AudioFile("output.wav") as source:
            audio_data = self.recognizer.record(source)
            try:
                text = self.recognizer.recognize_google(audio_data, language='zh-CN')
                self.text_box.insert(tk.END, text + "\n")
            except sr.UnknownValueError:
                self.text_box.insert(tk.END, "无法识别音频\n")
            except sr.RequestError as e:
                self.text_box.insert(tk.END, f"请求错误: {e}\n")

if __name__ == "__main__":
    VoiceRecorder()