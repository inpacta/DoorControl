<h1 align="center"> DoorControl 🚀 </h1>
<h4 align="center">
  Sistema completo de controle de acesso que roda em um microcontrolador ESP8266.
  Ele controla a abertura de portas e publica informações via MQTT, podendo ser acionado por diversos métodos.
</h4>
<br />

# :pushpin: Tabela de Conteúdos

- [:gear: Como usar](#gear-como-usar)
- [:page_facing_up: Licença](#page_facing_up-licença)

# :gear: Como usar

1. Clone este repositório:

   ```shell
   git clone https://github.com/inpacta/DoorControl.git
   ```

2. Configure as credenciais do Wi-Fi no arquivo `./src/main.cpp`:

   ```cpp
   const char* ssid = "NOME_DA_REDE";
   const char* password = "SENHA_DO_WIFI";
   ```

3. Configure as credenciais do Broker MQTT no arquivo `./src/main.cpp`:

   ```cpp
   const char* mqttServer = "endereco_do_broker";
   const int mqttPort = 1883;
   const char* mqttUser = "usuario"; // Opcional
   const char* mqttPassword = "senha"; // Opcional
   ```

4. Configure os tópicos para as informações `./src/main.cpp` :

   ```cpp
   const char *topicStatus_TAG = "SEU/TOPICO";
    const char *topicStatus_RF = "SEU/TOPICO";
   ```

5. Compile e faça o upload para o ESP8266 usando o Platformio.

    ```cpp
    /* O projeto acima, está baseado na utilização do Platformio, extensão do Visual Studio Code.
    */
    ```

    - [Como baixar e instalar Vscode e a extensão Platformio](https://www.youtube.com/watch?v=OZJ4niOrJ2k)
  
# :page_facing_up: Licença

Contribuições são bem-vindas! Sinta-se à vontade para abrir issues ou enviar pull requests.

Este projeto está sob a Licença MIT. Para mais informações sobre, <a href="/LICENSE">Clique aqui</a>

<img src="https://github.com/inpacta/.github/blob/main/profile/InPACTA-logo.png" alt="InPACTA Logo" width="100" align="right" />
