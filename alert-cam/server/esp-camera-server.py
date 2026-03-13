from flask import Flask, request, Response, jsonify
import threading
import base64

app = Flask(__name__)

images = []
lock = threading.Lock()
MAX_IMAGES = 5


@app.route("/")
def index():
    return """
    <html>
    <body style="background:black; color:white; text-align:center;">
        <h2>ESP32 Camera - Últimas 5 imagens</h2>

        <div id="imgs"></div>

        <script>
        async function update(){

            let r = await fetch("/images");
            let data = await r.json();

            let html = "";

            for(let img of data){
                html += `<img src="data:image/jpeg;base64,${img}" 
                         width="320" 
                         style="margin:10px;border:2px solid white;">`
            }

            document.getElementById("imgs").innerHTML = html;
        }

        setInterval(update, 1000);
        update();
        </script>
    </body>
    </html>
    """


@app.route("/upload", methods=["POST"])
def upload():

    img = request.data

    with lock:

        images.append(img)

        if len(images) > MAX_IMAGES:
            images.pop(0)

    print("image received:", len(img), "bytes")

    return "OK"


@app.route("/images")
def get_images():

    with lock:
        imgs = list(images)

    encoded = []

    for img in imgs:
        encoded.append(base64.b64encode(img).decode("utf-8"))

    return jsonify(encoded)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)