# Trabalho 1 - Fse
 
## Sobre o projeto:
 
#### Enunciado [aqui](https://gitlab.com/fse_fga/trabalhos-2021_2/trabalho-1-2021-2)
 
### Introdução
 
Este trabalho tem por objetivo a implementação de um sistema (que simula) o controle de um forno para soldagem de placas de circuito impresso (PCBs).
No trabalho, o aluno deverá desenvolver o software que efetua o controle de temperatura do forno utilizando dois atuadores para este controle: um resistor de potência de 15 Watts utilizado para aumentar a temperatura e uma ventoinha que puxa o ar externo (temperatura ambiente) para reduzir a temperatura do sistema.
Os comandos do usuário do sistema para definir a temperatura desejada serão controlados de três maneiras:
 
* Através de um Potenciômetro externo;
* Através de entrada de teclado no terminal;
* Seguindo uma curva de temperatura pré-definida em arquivo de configuração [Arquivo da Curva](curva_reflow.csv).
 
O controle da temperatura será realizado através da estratégia PID onde deverão estar disponíveis para o usuário o ajuste dos parâmetros Kp, Ki e Kd nas configurações do sistema (Via terminal).
 
### Componentes
 
O sistema como um todo é composto por:
 
* Ambiente fechado controlado com o resistor de potência e ventoinha;
* 01 Sensor DS18B20 (1-Wire) para a medição da temperatura interna (TI) do sistema;
* 01 Sensor BME280 (I2C) para a medição da temperatura externa (TE);
* 01 módulo Display LCD 16x2 com circuito I2C integrado (Controlador HD44780);
* 01 Conversor lógico bidirecional (3.3V / 5V);
* 01 Driver de potência para acionamento de duas cargas;
* 01 ESP32;
* 01 Potenciômetro (Para definir a temperatura de referência TR);
* 01 Switch (Para definir a estratégia de controle);
* 01 Raspberry Pi 4;
 
## Como executar
 
### Compilação
Para compilar, na página do repositório, utilize o seguinte comando:
 
##### `make`
 
### Executar
 
##### `make run`
 
## Como utilizar
 
### 1. Selecione um dos modos:
 
* __Terminal__
 * Solicita uma temperatura de referência e a mantém até a finalização da aplicação ou uma mudança de modo via comandos de usuários.
 
* __Potênciometro__
 * A temperatura de referência será guiada pelo potenciômetro acoplado ao sistema.
* __Curva de temperatura (Reflow)__
 * A temperatura de referência será guiada pelo [arquivo csv](curva_reflow.csv) na raiz do projeto.
 
### 2. Defina os valores para o cálculo do PID:
* Kp
* Ki
* Kd
 
### 3. Mudança de modo após inicialização:
Caso deseje mudar o modo será necessário enviar comandos de usuário via [dashboard](https://embarcados.ngrok.io/dashboard/2b279690-97f1-11ec-8b74-d591aa552791?publicId=8bda2d10-1933-11eb-b367-b1e3090cb5c1)
 
### 4. Finalizar o app:
Para finalizar o app, solicite o comando de desligar via dashboard ou pressione ctrl+c no terminal.
 
### __Obs__:
* A todo momento a aplicação cria logs no terminal e salva informações, em um csv ("logs.csv"), das temperaturas (interna, externa, de referência, referência solicitada pelo usuário, potência de acionamento da ventoinha e do resistor e o valor calculado do PID).
* Temperatura interna, externa, de referência e modo do programa são atualizados constantemente no LCD.
 
## Experimentos
 
### Potenciômetro:

#### Dados dos gráficos abaixo [aqui](assets/log-potenciometro-final.csv).

<p align="center">
  <img src="assets/Potenciometro (Potenciometro).png" width="850" height="auto" alt="Curva de temperatura (Modo Reflow)"/>
</p>

<p align="center">
  <img src="assets/Sinal de controle (Potenciometro).png" width="850" height="auto" alt="Sinal de controle (Modo Reflow)"/>
</p>

### Curva de temperatura (Reflow):

#### Dados dos gráficos abaixo [aqui](assets/log-reflow-final.csv).

<p align="center">
  <img src="assets/Curva de temperatura (Modo Reflow).png" width="850" height="auto" alt="Curva de temperatura (Modo Reflow)"/>
</p>

<p align="center">
  <img src="assets/Sinal de controle (Modo Reflow).png" width="850" height="auto" alt="Sinal de controle (Modo Reflow)"/>
</p>