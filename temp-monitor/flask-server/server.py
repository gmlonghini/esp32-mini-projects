from flask import Flask, request, jsonify, render_template_string
from datetime import datetime

app = Flask(__name__)

history = []

MAX_RECORDS = 20

HTML_PAGE = """
<!DOCTYPE html>
<html>
<head>
    <title>Monitor de Temperatura</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <meta http-equiv="refresh" content="5">
    <style>
        body { font-family: Arial; text-align: center; }
        .current {
            font-size: 48px;
            color: red;
            margin: 20px;
        }
        .chart-container {
            width: 80%;
            margin: auto;
        }
    </style>
</head>
<body>

<h1>Temperatura Atual</h1>

{% if history %}
<div class="current">
    {{ history[0]['temp'] }} °C
</div>
<p>{{ history[0]['time'] }}</p>
{% else %}
<p>Sem dados</p>
{% endif %}

<h2>Últimas 20 leituras</h2>

<div class="chart-container">
    <canvas id="tempChart"></canvas>
</div>

<script>
const labels = {{ history | map(attribute='time') | list | tojson }};
const dataValues = {{ history | map(attribute='temp') | list | tojson }};

// Como os dados são inseridos no início, invertendo para ordem cronológica
labels.reverse();
dataValues.reverse();

const ctx = document.getElementById('tempChart').getContext('2d');

new Chart(ctx, {
    type: 'line',
    data: {
        labels: labels,
        datasets: [{
            label: 'Temperatura (°C)',
            data: dataValues,
            borderWidth: 2,
            tension: 0.2,
            fill: false
        }]
    },
    options: {
        animation: false,
        scales: {
            y: {
                beginAtZero: false
            }
        }
    }
});
</script>

</body>
</html>
"""

@app.route("/data", methods=["POST"])
def receive_data():
    global history

    data = request.get_json()

    if not data or "temperature" not in data:
        return jsonify({"status": "error"}), 400

    try:
        temp = float(data["temperature"])
    except ValueError:
        return jsonify({"status": "error"}), 400

    record = {
        "temp": float(f"{temp:.4f}"),
        "time": datetime.now().strftime("%H:%M:%S")
    }

    history.insert(0, record)
    history = history[:MAX_RECORDS]

    print(f"Recebido: {record['temp']:.4f} °C às {record['time']}")

    return jsonify({"status": "ok"})


@app.route("/")
def index():
    return render_template_string(HTML_PAGE, history=history)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
