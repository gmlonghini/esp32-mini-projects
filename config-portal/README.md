# ESP32 LED Config Portal

Projeto ESP-IDF que controla o período de um LED e oferece uma interface web para configurá-lo. O valor escolhido é salvo na NVS e permanece após reinicializações ou desligamentos.

O firmware opera em dois modos exclusivos, selecionados no boot por um sensor touch TTP223:

- **Modo LED:** executa somente a task que pisca o LED com o período salvo.
- **Modo de configuração:** inicia um Access Point e o servidor web para alterar o período.

## Hardware

- ESP32
- Módulo touch TTP223
- LED
- Resistor de 220 a 330 Ω para o LED
- Protoboard e jumpers

### Ligações

| Componente | Pino do componente | ESP32 |
|---|---|---|
| TTP223 | VCC | 3V3 |
| TTP223 | GND | GND |
| TTP223 | SIG | GPIO27 |
| LED | Ânodo (+), através do resistor | GPIO13 |
| LED | Cátodo (-) | GND |

> Alimente o TTP223 em 3,3 V para manter seu sinal de saída compatível com o ESP32.

## Como usar

### Modo LED

Ligue ou reinicie o ESP32 sem tocar no TTP223. O LED usará o período armazenado na NVS. Caso ainda não exista uma configuração válida, o padrão será `1000 ms`.

O período representa o tempo entre cada troca de estado: com `1000 ms`, o LED permanece 1 segundo ligado e 1 segundo desligado.

### Modo de configuração

1. Mantenha o TTP223 tocado enquanto liga ou reinicia o ESP32.
2. Conecte-se à rede Wi-Fi:
   - SSID: `ESP32_AP`
   - Senha: `12345678`
3. Acesse `http://192.168.4.1`.
4. Informe um período entre `100` e `10000 ms` e pressione **Salvar período**.
5. Solte o TTP223 e reinicie o ESP32 para executar o modo LED com o novo valor.

## Compilar e gravar

Requer o ESP-IDF configurado no terminal.

```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Substitua `/dev/ttyUSB0` pela porta serial da sua placa.

## Estrutura

```text
.
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   ├── main.c
│   └── web_page.html
└── README.md
```

- `main.c`: seleção do modo, task do LED, NVS, Wi-Fi e servidor HTTP.
- `web_page.html`: interface incorporada diretamente no firmware durante a compilação.
