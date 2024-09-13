from direct.showbase.ShowBase import ShowBase
from panda3d.core import DirectionalLight, AmbientLight, Vec3, Vec4, Filename, Point3

class MyApp(ShowBase):
    def __init__(self):
        ShowBase.__init__(self)

        # 加载FBX模型
        self.vtuber_model = self.loader.loadModel(Filename.fromOsSpecific("vtuber2.fbx"))
        self.vtuber_model.reparentTo(self.render)
        self.vtuber_model.setScale(1, 1, 1)
        self.vtuber_model.setPos(0, 0, 0)

        # 设置摄像机位置和角度
        self.camera.setPos(100, -1000, 5000)  # 设置摄像机在模型前面，稍微向上
        self.camera.lookAt(self.vtuber_model)  # 让摄像机看向模型

        # 添加方向光
        dlight = DirectionalLight('dlight')
        dlight.setColor(Vec4(1, 1, 1, 1))  # 白色光
        dlight_node = self.render.attachNewNode(dlight)
        dlight_node.setHpr(45, -45, 0)  # 设置光照方向
        self.render.setLight(dlight_node)

        # 添加环境光
        alight = AmbientLight('alight')
        alight.setColor(Vec4(0.3, 0.3, 0.3, 1))  # 灰色环境光，增加整体亮度
        alight_node = self.render.attachNewNode(alight)
        self.render.setLight(alight_node)

app = MyApp()
app.run()