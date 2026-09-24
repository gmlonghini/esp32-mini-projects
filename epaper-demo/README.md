# ESP32 + WeAct e-paper 2,13"

Exemplo ESP-IDF para o modulo monocromatico WeAct de 2,13", com controlador
SSD1680 e resolucao visivel de 122 x 250 pixels. A interface e exibida em
paisagem (`250 x 122`), mostra informacoes do projeto e gera um `Random ID`
diferente a cada inicializacao. Depois da atualizacao, o display entra em
_deep sleep_.

O componente `components/lib_ssd1680` foi copiado integralmente, sem alteracoes
nos arquivos `.c` e `.h`, do projeto
[`piotr-wrobel/ssd1680_esp32_c_driver`](https://github.com/piotr-wrobel/ssd1680_esp32_c_driver),
commit `a919e4a16a8c07ab1d931f1e105b8bd760094db8`. Esse componente deriva do
[`DECE2183/esp32-ssd1680-driver`](https://github.com/DECE2183/esp32-ssd1680-driver)
e acrescenta fontes bitmap e a funcao `ssd1680_display_string()`.

O codigo importado e distribuido sob GPL-3.0; uma copia da licenca esta em
`components/lib_ssd1680/LICENSE`.

## Ligacoes

Use 3,3 V na alimentacao e nos sinais:

| WeAct e-paper |     ESP32 | Funcao          |
| ------------- | --------: | --------------- |
| `VCC`         |     `3V3` | Alimentacao     |
| `GND`         |     `GND` | Terra           |
| `SDA`         | `GPIO 23` | SPI MOSI        |
| `SCL`         | `GPIO 18` | SPI SCLK        |
| `CS`          |  `GPIO 5` | Chip select     |
| `BUSY`        |  `GPIO 4` | Display ocupado |
| `RES`         | `GPIO 16` | Reset           |
| `D/C`         | `GPIO 17` | Comando/dados   |

Embora estejam marcados como `SDA` e `SCL`, esses dois sinais pertencem ao
barramento SPI. O modulo nao utiliza I2C e nao precisa de MISO.

## Compilar e gravar

Com o ambiente ESP-IDF carregado:

```bash
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Troque `/dev/ttyUSB0` pela porta serial correta. Para sair do monitor, use
`Ctrl+]`.
