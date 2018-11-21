  #include <SoftwareSerial.h>
  #include <LiquidCrystal.h>
  
  LiquidCrystal lcd (9, 7, 8, 12, 10, 11);
  SoftwareSerial BTSerial (1, 0); // RX | TX
  
  #define LED_Alerta 6    // Pin is connected to the LED ALERTA
  #define LED_Direito 5   // Pin is connected to the LED DIREITO
  #define LED_Esquerdo 13 // Pin is connected to the LED ESQUERDO
  
  int acaba = 3;
  int desce = 4;
  int sobe = 2;
  
  bool stim = true;
  
  int BTN_Down = HIGH;
  int BTN_Up = HIGH;
  int lastReadingDown = HIGH;
  int lastReadingUp = HIGH;
  
  long lastSwitchTimeDown = 0;
  long lastSwitchTimeUp = 0;
  long longSwitchTime = 500;
  long shortSwitchTime = 100;
  long switchTimeDown = 500;
  long switchTimeUp = 500;
  
  String flag;
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  void setup () {
  
    lcd.begin (16, 2);
  
    bool conexao = false;
    digitalWrite (acaba, HIGH);
    digitalWrite (desce, HIGH);
    digitalWrite (sobe, HIGH);
    pinMode (acaba, INPUT_PULLUP);
    pinMode (desce, INPUT_PULLUP);
    pinMode (sobe, INPUT_PULLUP);
    pinMode (LED_Alerta, OUTPUT);   // Sets pin as OUTPUT.
    pinMode (LED_Direito, OUTPUT);  // Sets pin as OUTPUT.
    pinMode (LED_Esquerdo, OUTPUT); // Sets pin as OUTPUT.
  
    Serial.begin (9600);
    Serial.flush (); // Clear receive buffer.
  
    inicializacao ();
  
    while (conexao == false) {
      if (Serial.available ()) {
        flag = Serial.readString ();
        //lcd.print (flag);
        if (flag.equals ("a")) {
          conexao = true;
          lcd.clear ();
          lcd.print ("Iniciando estimulacao");
          delay (1000);
        }
      }
    }
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  void loop () {
  
    int state_0 = 0;
    int state_1 = 1;
    digitalWrite (LED_Alerta, LOW);
  
    while (stim) {
      if (digitalRead (sobe) == LOW) {
        digitalWrite (sobe, HIGH);
        state_0 = 1;
        lcd.clear ();
        lcd.print ("Extensao");
        digitalWrite (LED_Direito, HIGH);
      }
      else if (digitalRead (desce) == LOW) {
        digitalWrite (desce, HIGH);
        state_0 = 2;
        lcd.clear ();
        lcd.print ("Flexao");
        digitalWrite (LED_Esquerdo, HIGH);
  
      }
      else if (digitalRead (acaba) == LOW) {
        digitalWrite (acaba, HIGH);
        state_0 = 3;
  
      }
      else {
        state_0 = 0;
      }
      if (state_1 != state_0) {
        Serial.print (state_0);
        state_1 = state_0;
        lcd.clear ();
        lcd.print ("    Pressione   ");
        lcd.setCursor (0, 1);
        lcd.print ("<-Flex     Ext->");
        digitalWrite (LED_Esquerdo, LOW);
        digitalWrite (LED_Direito, LOW);
      }
  
      if (state_0 == 3) {
        Serial.print (state_0);
        stim = false;
        lcd.clear ();
        lcd.print ("Fim");
  
      }
  
      delay (30);
    }
    delay (500);
    digitalWrite (LED_Alerta, HIGH);
    delay (500);
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  /**
     [inicializacao description]
     @return {[type]} [description]
  */
  void inicializacao () {
  
    int corrente_isq = 10;
    int corrente_quad = 10;
    int freq = 50;
    int mode = 2;
    int pw = 200;
  
    bool conexao = false;
  
  
    // para a funcao
    int passo;
    int variavel;
    String text;
    String unidade;
  
    //verificando conexão //////////////////////////////////////////////////////////////////////////////////
  
    lcd.clear ();
    lcd.setCursor (0, 1);
    lcd.print (" EMA FES-Rowing ");
    delay(5000);
    lcd.setCursor (0, 0);
    lcd.print ("   Conectando...");
  
     //conexao = false;
     while (conexao == false){
       if (Serial.available ()){
        flag = Serial.readString ();
        if (flag.equals ("a")){
          conexao = true;
        }
      }
     }
  
     Serial.write (1);
     lcd.clear ();
     lcd.print ("Conectado!");
     delay (1000);
  
    // pegando parametros de quantidade de canais, corrente e largura de pulso (pw) ////////////////////////
  
    //modo de operação
    text = "Qtd de Canais:";
    variavel = mode;
    passo = 2;
    unidade = "CHs";
    mode =  func_set_parametro (variavel, passo, text, unidade);
    delay (500);
  
  
    //corrente1 - quadriceps
    text = "Corrente Quad.:";
    variavel = corrente_quad;
    passo = 2;
    unidade = "mA (CH [1,2])";
    corrente_quad =  func_set_parametro (variavel, passo, text, unidade);
    delay (500);
  
    //corrente2 - isquiotibiais
    if (mode == 4) {
      text = "Corrente Isq.:";
      variavel = corrente_isq;
      passo = 2;
      unidade = "mA (CH[3,4])";
      corrente_isq =  func_set_parametro (variavel, passo, text, unidade);
      delay (500);
    }
  
    //largura de pulso
    text = "Largura de Pulso:";
    variavel = pw;
    passo = 10;
    unidade = "ms";
    pw =  func_set_parametro (variavel, passo, text, unidade);
    delay (500);
  
    //Frequencia
    text     = "Frequencia:";
    variavel = freq;
    passo    = 5;
    unidade  = "Hz";
    freq     = func_set_parametro (variavel, passo, text, unidade);
    delay (500);
  
  
    // enviando dados pela serial (bluetooth) //////////////////////////////////////////////////////////////
    Serial.print ("c"); //marcador de corrente_quad
    func_dim_string_to_Stim(corrente_quad);
  
    Serial.print ("x"); //marcador de corrente_isq
    func_dim_string_to_Stim(corrente_isq);
  
    Serial.print ("p"); //marcador de largura de pulso
    func_dim_string_to_Stim(pw);
  
    Serial.print ("f"); //marcador de frequecia
    func_dim_string_to_Stim(freq);
  
    Serial.print ("m"); //marcador do modo
    func_dim_string_to_Stim(mode);
  
    lcd.clear ();
    lcd.print ("Enviando");
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  int qtdAlgarismos (int numero) {
    int cont = 0;
    while (numero != 0) {
      // n = n/10
      numero /= 10;
      cont++;
    }
    return cont;
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  /**
     [func_set_parametro seta os parâmetros através dos botões BTN_Up e BTN_Up ]
     @param  {[int]}     variavel      [Variável a ser alterada]
     @param  {[int]}     passo         [passo da alteração ao apertar botões]
     @param  {[String]}  text          [texto informado no display]
     @param  {[String]}  unidade       [unidade informada no display]
     @return {[int]}        [Salva o valor da variável]
  */
  int func_set_parametro (int variavel, int passo, String text, String unidade) {
    lcd.clear ();
    lcd.print (text);
    while (digitalRead (acaba) == HIGH) {
  
      lcd.setCursor (0, 1);
      lcd.print (variavel);
      lcd.print (" ");
      lcd.print (unidade);
      lcd.print ("   ");
  
  
      BTN_Up = digitalRead (sobe);
      BTN_Down = digitalRead (desce);
      delay (100);
      if (BTN_Up == LOW && ((((millis () - lastSwitchTimeUp) > switchTimeUp) && lastSwitchTimeUp != 0) || lastReadingUp == HIGH)) {
        //digitalWrite (sobe, HIGH);
        if (((millis () - lastSwitchTimeUp) > switchTimeUp) && lastSwitchTimeUp != 0) {
          switchTimeUp = shortSwitchTime;
        }
        lastSwitchTimeUp = millis ();
        lastReadingUp = BTN_Up;
        variavel = variavel + passo;
        if (text == "Qtd de Canais:") {
          if (variavel > 4) {
            variavel = 4;
          }
        }
  
        delay (100);
      }
      if (BTN_Up == HIGH) {
        lastReadingUp = BTN_Up;
        switchTimeUp = longSwitchTime;
        lastSwitchTimeUp = 0;
      }
      if (BTN_Down == LOW && ((((millis () - lastSwitchTimeDown) > switchTimeDown) && lastSwitchTimeDown != 0) || lastReadingDown == HIGH)) {
        //digitalWrite (desce, HIGH);
        if (((millis () - lastSwitchTimeDown) > switchTimeDown) && lastSwitchTimeDown != 0) {
          switchTimeDown = shortSwitchTime;
        }
        lastSwitchTimeDown = millis ();
        lastReadingDown = BTN_Down;
        variavel = variavel - passo;
        if (text == "Qtd de Canais:") {
          if (variavel < 2) {
            variavel = 2;
          }
        }
        if (variavel < 0) {
          variavel = 0;
        }
        delay (100);
      }
      if (BTN_Down == HIGH) {
        lastReadingDown = BTN_Down;
        switchTimeDown = longSwitchTime;
        lastSwitchTimeDown = 0;
      }
    }
    return variavel;
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  /**
     [func_dim_string_to_Stim Configura o envio dos parametros sempre com 3 algarismo]
     @param  {[int]}  entrada       [Parametro a ser convertido]
     @return {[int]}     [Parâmetro convertido em 3 algarismo]
  */
  void func_dim_string_to_Stim(int entrada) {
    if (qtdAlgarismos (entrada) == 3) {
      Serial.print (entrada);
    } else if (qtdAlgarismos (entrada) == 2) {
      Serial.print (0);
      Serial.print (entrada);
    } else if (qtdAlgarismos (entrada) == 1) {
      Serial.print (0);
      Serial.print (0);
      Serial.print (entrada);
    }
  }
