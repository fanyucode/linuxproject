import cv2
import numpy as np
import oss2

# 初始化OpenCV的视频捕获
cap = cv2.VideoCapture(0)

# 加载模型（这里只是一个示例，你可能需要根据你的模型进行调整）
net = cv2.dnn.readNetFromDarknet("yolov3.cfg", "yolov3.weights")
layer_names = net.getLayerNames()
output_layers = [layer_names[i[0] - 1] for i in net.getUnconnectedOutLayers()]

# 阿里云OSS配置
auth = oss2.Auth('<Your AccessKeyID>', '<Your AccessKeySecret>')
bucket = oss2.Bucket(auth, 'http://oss-cn-hangzhou.aliyuncs.com', '<Your Bucket Name>')

while True:
    ret, frame = cap.read()
    if not ret:
        break

    height, width, channels = frame.shape

    # 对帧进行推理
    blob = cv2.dnn.blobFromImage(frame, 0.00392, (416, 416), (0, 0, 0), True, crop=False)
    net.setInput(blob)
    outs = net.forward(output_layers)

    # 获取检测结果
    class_ids = []
    confidences = []
    boxes = []
    for out in outs:
        for detection in out:
            scores = detection[5:]
            class_id = np.argmax(scores)
            confidence = scores[class_id]
            if confidence > 0.5: 
                # 目标检测
                center_x = int(detection[0] * width)
                center_y = int(detection[1] * height)
                w = int(detection[2] * width)
                h = int(detection[3] * height)
                x = int(center_x - w / 2)
                y = int(center_y - h / 2)
                boxes.append([x, y, w, h])
                confidences.append(float(confidence))
                class_ids.append(class_id)

    # 使用NMS消除低置信度的重叠框
    indexes = cv2.dnn.NMSBoxes(boxes, confidences, 0.5, 0.4)

    # 检查是否检测到人（在COCO数据集中，人的ID为0）
    if 0 in class_ids:
        # 保存帧为图片
        cv2.imwrite("detected_person.jpg", frame)
        
        # 上传到阿里云OSS
        with open("detected_person.jpg", 'rb') as f:
            bucket.put_object('<Your Object Name>', f)
cap.release()
cv2.destroyAllWindows()
