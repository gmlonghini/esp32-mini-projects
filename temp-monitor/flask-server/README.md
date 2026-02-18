# ESP32 Temp Monitor – Server

Servidor local simples para receber dados de temperatura do ESP32 e exibir:

- Temperatura atual (em destaque)
- Horário da última leitura
- Histórico dos últimos 50 registros

O servidor é feito em **Python + Flask** e roda em um **ambiente virtual (venv)**.

---

## Requisitos

- Python 3.10 ou superior
- Pacote venv instalado

No Ubuntu/Debian:

```bash
sudo apt install python3-venv
```

---

## Passo 1 – Criar o ambiente virtual

```bash
python3 -m venv venv
```

Isso cria uma pasta chamada `venv` com um ambiente Python isolado.

---

## Passo 2 – Ativar o ambiente

Linux / macOS:

```bash
source venv/bin/activate
```

Você verá o prefixo `(venv)` no terminal.

---

## Passo 3 – Instalar as dependências

```bash
pip install flask
```

Opcional (para reprodutibilidade):

```bash
pip freeze > requirements.txt
```

---

## Passo 4 – Executar o servidor

```bash
python server.py
```

O servidor iniciará em:

```
http://localhost:5000
```

Se quiser acessar de outro dispositivo na rede:

```
http://IP_DO_SEU_PC:5000
```

---
