# Sistema de Estacionamento - Parte Física (Arduino)

Este projeto implementa a parte física de um sistema de estacionamento utilizando **Arduino** para controlar sensores, atuadores e painéis de exibição de status. O sistema é responsável por detectar o status das vagas e interagir com um **ESP32** para comunicação com a aplicação web.

## Arquitetura Orientada a Objetos

O código foi escrito utilizando uma abordagem **orientada a objetos** para facilitar a escalabilidade e adição de novos componentes físicos ou atuadores no futuro. Cada componente do sistema possui sua própria classe, encapsulando suas propriedades e métodos, o que permite:
- Manutenção mais simples e organizada.
- Facilitar a inclusão de novos sensores ou atuadores.
- Um código principal (loop) mais limpo e legível, onde os métodos são chamados de maneira organizada, melhorando a clareza e a eficiência do fluxo de execução.

Por exemplo, cada sensor ultrassônico, servo motor e o painel LCD possuem classes separadas, com métodos para atualizar seu estado e reagir a mudanças no ambiente.

## Funcionalidades

1. **Detecção de vagas com sensores ultrassônicos**: 
   - Cada vaga tem um sensor ultrassônico que detecta a presença de um veículo a uma distância de até **10 cm**.
   - O status da vaga é atualizado com base nessa detecção:
     - **Vaga ocupada**: O LED vermelho acende.
     - **Vaga disponível**: O LED verde acende.
   - O status das vagas é armazenado em variáveis e enviado para a **Serial2** para ser processado pelo ESP32.

2. **Painel de vagas disponíveis**:
   - Um **painel de 7 segmentos** exibe o total de vagas disponíveis em tempo real.

3. **Controle de cancela com servo motor**:
   - Um **servo motor** controla a abertura e fechamento da cancela do estacionamento.
   - A cancela é aberta e fechada de acordo com os comandos recebidos da aplicação web, através da serial 2.

4. **Exibição de mensagens no painel LCD**:
   - Um **painel LCD** exibe mensagens enviadas pelos administradores do sistema web.
   - As mensagens são recebidas pela Serial2 e exibidas no painel.

## Fluxo de Operação

### 1. Detecção de Vagas e Atualização do Status
Cada vaga é monitorada por um **sensor ultrassônico**, que verifica a presença de um objeto. Quando um objeto é detectado a menos de **10 cm**, o Arduino altera o status da vaga correspondente:
- Se o objeto está presente, a variável `vacancyOneStatus` é definida como `false` (vaga ocupada), e o **LED vermelho** acende.
- Se o objeto não está presente, a variável `vacancyOneStatus` é definida como `true` (vaga disponível), e o **LED verde** acende.

O status de todas as vagas é atualizado e o total de vagas disponíveis é exibido no **painel de 7 segmentos**.

### 2. Envio de Status via Serial2
Os status das vagas são enviados para o **ESP32** através da **Serial2**. O formato da mensagem é uma string no formato:
```
vacancyOneStatus:true
```
O ESP32 coleta essas informações e as envia para a aplicação web para atualização em tempo real.

### 3. Exibição de Mensagens e Controle de Cancela
O Arduino também recebe comandos da aplicação web via serial 2 para exibir mensagens no **painel LCD** e para abrir ou fechar a cancela do estacionamento. A comunicação é feita pela **Serial2**, onde o ESP32 envia informações no formato:
```
gateStatus:true
messageAdm:Bem-vindo!
```

- A variável `gateStatus` define se a cancela deve ser aberta ou fechada.
- A variável `messageAdm` contém a mensagem que será exibida no **painel LCD**.

Quando um comando de abertura da cancela é recebido, o **servo motor** aciona a cancela, desde que a mesma já não esteja aberta.

### 4. Processamento das Mensagens
O Arduino processa as mensagens recebidas da Serial2 verificando se o conteúdo da mensagem começa com o nome da variável correspondente. Por exemplo:
```
vacancyOneStatus:true
```
Neste caso, o Arduino verifica se existe a chave `vacancyOneStatus`, e se confirmada, ele coleta o valor após os dois pontos (`:`), que neste caso é `"true"`, e atualiza o status da vaga.

## Componentes Utilizados

- **Arduino Mega**: Controlador principal da parte física.
- **ESP32**: Responsável pela comunicação com a aplicação web.
- **Sensor Ultrassônico**: Detecta a presença de veículos nas vagas.
- **LEDs (Verde/Vermelho)**: Indicadores do status das vagas.
- **Painel de 7 Segmentos**: Exibe o número de vagas disponíveis.
- **Servo Motor**: Controla a abertura e fechamento da cancela.
- **Painel LCD**: Exibe mensagens personalizadas enviadas pela aplicação web.
  
## Estrutura de Comunicação

- **Serial2 (Arduino ↔ ESP32)**: Usada para troca de informações sobre o status das vagas e comandos para abertura/fechamento da cancela, além de mensagens para o painel LCD.
  
As mensagens são enviadas no formato `chave:valor`, e processadas pelo Arduino através da comparação do nome da variável e extração do valor após os dois pontos (`:`).

## Exemplo de Código para Processamento de Mensagens

```cpp
String inputString = "vacancyOneStatus:true";
String variableName = "vacancyOneStatus";

if (inputString.startsWith(variableName)) {
    String value = inputString.substring(inputString.indexOf(":") + 1);
    // Processar o valor (neste caso "true")
}
```

## Organização do Código com Orientação a Objetos

Para garantir a escalabilidade e facilitar futuras adições de novos componentes ou sensores, o código foi desenvolvido usando a **orientação a objetos**. Cada componente, como sensores, servo motor e painel LCD, painel 7 segmentos, possui sua própria **classe**, com métodos e propriedades específicos, tornando o código mais modular e de fácil manutenção.

### Exemplo de Classe

```cpp
class SensorUltrassonico {
public:
    int pinTrigger;
    int pinEcho;
    
    SensorUltrassonico(int trigger, int echo) {
        pinTrigger = trigger;
        pinEcho = echo;
        pinMode(pinTrigger, OUTPUT);
        pinMode(pinEcho, INPUT);
    }

    bool detectarObjeto() {
        // Lógica para detectar presença de objeto
    }
};
```

No **loop principal**, os métodos das classes são chamados para garantir a organização e clareza do código:

```cpp
void loop() {
    sensorUltrassonico1.detectarObjeto();
    painelLCD.exibirMensagem();
    cancela.abrirOuFechar();
}
```

Dessa forma, o código principal fica mais limpo e organizado, facilitando sua compreensão e expansão.

---

Este README fornece uma visão detalhada da parte física do sistema de estacionamento, incluindo a implementação orientada a objetos para facilitar a escalabilidade e manutenção futura.
