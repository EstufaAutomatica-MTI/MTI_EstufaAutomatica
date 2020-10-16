# MTI_EstufaAutomatica
### Autores: 
- Adriano André da Silva ([GitHub](https://github.com/Adriano-Andre/))
- Bruno Gabriel Berwanger ([GitHub](https://github.com/BrunoGabrielBerwanger/))
- Fabrício Pereira Lutz ([GitHub](https://github.com/FabricioLutz/))
- Jordana Pedroso Dutra ([GitHub](https://github.com/DutraJordana/))
### Professor: Marcos Zuccolotto ([GitHub](https://github.com/Zucco1997/)), Disciplina: Micros II, Curso Técnico de Eletrônica - 4° ANO
### [Fundação Escola Técnica Liberato Salzano Vieira da Cunha](https://liberato.com.br)

## PROJETO
O projeto Estufa Automática tem por objetivo desenvolver três estufas, medindo e atuando sobre a temperatura, luminosidade e umidade do solo. Essa implementação fará uso do protocolo MQTT, tornando as estufas em objetos virtuais para IoT, o que permitirá um controle à distância das estufas (que estarão colocadas em duas cidades) e, além disso, permitirá uma análise de consumo por parte de cada estufa à partir do consumo medido nas. Dessa maneira, cria-se a possibilidade de identificar falhas na aplicação, qual estufa obteve menores gastos e criar alertas em caso de defeitos.

-	Conforme comentado no vídeo, a estufa gama apresentou falhas no sensor de corrente ACS712. Ao perceber que o sensor estava acurando a corrente de maneira errônea e que, principalmente, um mesmo sensor demoraria um certo tempo para ser encomendado, optamos por continuar com a medição fora do comum e usá-la para exemplificar o desligamento de segurança para sobrecorrente, conforme também explicado no vídeo.

-	**ATENÇÃO:** o arquivo "flows.json", ao ser importado, pode por vezes desformatar o dashboard, trocando a posição dos textos de potência elétrica na aba Consumo Elétrico. Em caso de erro, basta abrir a opção na aba de configurações *Dashboard*, selecionar ao lado do nome da tabela *MeuTrecoNaInternet* a ferramenta *Layout* e lá os textos podem ser rearranjados rapidamente.

**Veja abaixo o vídeo demonstrativo do projeto:**
[![Click to watch the video](https://img.youtube.com/vi/yS3YVaWQQGw/maxresdefault.jpg)](https://youtu.be/yS3YVaWQQGw)

## DIAGRAMAS ESQUEMÁTICOS
-	Diagrama em blocos da arquitetura do sistema.
	<p align="center">
		<img src="https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/Diagram.png?raw=true"/>
	</p>
-	Esquema elétrico das três estufas.
	<p align="center">
		<img src="https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/Schematic.png?raw=true"/>
	</p>
-	Fotos da Estufa Alfa.
	<p align="center">
		<img src="https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/picAlfa1.jpeg?raw=true"/>
		<img src="https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/picAlfa2.jpeg?raw=true"/>
		<img src="https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/picAlfa3.jpeg?raw=true"/>
		<img src="https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/picAlfa4.jpeg?raw=true"/>
	</p>
-	Fotos da Estufa Beta.
	<p align="center">
		<img src="https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/picBeta1.jpg?raw=true"/>
		<img src="https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/picBeta2.jpg?raw=true"/>
		<img src="https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/picBeta3.jpg?raw=true"/>
	</p>
-	Fotos da Estufa Gama.
	<p align="center">
		<img src="https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/picGama1.jpeg?raw=true"/>
		<img src="https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/picGama2.jpeg?raw=true"/>
		<img src="https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/picGama3.jpeg?raw=true"/>
		<img src="https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/picGama4.jpeg?raw=true"/>
	</p>
-	**[DOXYGEN](https://github.com/EstufaAutomatica-MTI/MTI_EstufaAutomatica/blob/master/html/index.html)**

## DESCRIÇÃO DA ARQUITETURA

Nesse projeto foram desenvolvidas três dispositivos idênticos, as estufas, dotadas de sensores de umidade, luz e temperatura, juntamente de atuadores que operam sobre essas variáveis. Sua estrutura foi feita em isopor, levando em conta o manuseio prático para adaptação de sensores e atuadores na carcaça e, além disso, a capacidade de reter a temperatura. Para controlar os valores padrões a serem alcançados na estufa e para visualização das condições atuais dela, empregamos o controle e medições em uma aplicação construída no STM32 CubeMonitor, usando gráficos vetoriais escalonáveis [SVG](https://github.com/bartbutenaers/node-red-contrib-ui-svg) para uma interface visualmente mais atrativa e que possibilitasse uma maior intuitividade ao usar o ‘dashboard’ da aplicação. Essa aplicação foi desenvolvida para ser “amigável” ao usuário, ou seja, de forma a ter um uso simples e intuitivo e que prevê soluções para possíveis erros de uso. 
	
A aplicação serve para conectar os dispositivos, por meio da porta serial, e envia/publica os dados obtidos para um tópico (uso do protocolo MQTT) voltado aos sensores (iotTro/EstufaAutomatica/Sensores). Os dados das três estufas são enviados para o mesmo tópico, então a aplicação, ao receber esses dados, faz a devida separação pelo nome das estufas e, em seguida, expõe os dados na interface. Os atuadores são controlados por um grupo da aplicação em que os valores são selecionados, esses dados são revisados e por fim enviados ao tópico iotTro/EstufaAutomatica/Atuadores. Os valores selecionados são enviados para os dispositivos por meio da porta serial. Para o uso do protocolo MQTT foi usado o broker [Eclipse](mqtt.eclipse.org), com a porta encriptada 8883, porém sem certificação.
	
Resumidamente, os dados processados pelo ARM, na estufa, são enviados pela serial para o PC, e do PC para o broker Eclipse. O broker irá enviar os dados dessa e das demais estufas para os ‘Subscribers’ do tópico, que, ao receber esses dados, lidam com eles para sua amostragem na interface do Node. Para controlar os valores ideais da estufa, o usuário delimita novos parâmetros na interface e os envia para o tópico dos atuadores, esses novos parâmetros, ao chegarem no broker, são enviados aos ‘Subscribers’ desse tópico, que, por sua vez, irão receber os dados no PC e enviarão esses valores ao ARM, que os interpreta e seta como ideais.

## RELATO DE DESENVOLVIMENTO
-	**Estratégias de desenvolvimento**

Nossa estratégia de desenvolvimento se iniciou em agosto, com o estudo de potenciais sensores, atuadores e métodos para tornar a ideia em algo real, com cada um se responsabilizando por uma tarefa. Com todos os sensores e atuadores escolhidos e testados, passamos a fazer encontros semanais, nos horários que tínhamos disponíveis, para unir todos em um código, integrando-o ao Node. Nossa meta era de ter o código e a interface desenvolvidas até a última semana do mês de setembro, e usar essa ultima semana para testes e pequenos ajustes. Com a prorrogação da data de entrega, decidimos trabalhar mais no código, desenvolvendo o desligamento e religamento à distância, controle da cor da fita de led e melhorando seu desempenho. Além disso, usamos o tempo extra para inserir o SVG na interface e desenvolver notificações de erro de digitação e outros diferenciais para melhorar seu uso. Os códigos foram sendo desenvolvidos sincronamente, para torná-los o mais coesos possíveis entre si, tendo em vista as limitações tanto do ARM, quanto do Node Red. 
	
As tarefas foram divididas conforme a disponibilidade de cada um na semana, mas sempre optando por desenvolver o máximo de coisas juntos, em chamadas online, para um melhor aprendizado e um uso mais proveitoso do tempo, uma vez que cada um pensava em uma abordagem diferente para o mesmo problema, assim podendo escolher qual era a mais efetiva. Desenvolvemos os códigos no início das semanas e durante seu decorrer íamos testando falhas, melhorias e outras perspectivas, fazendo pequenas atualizações. Assim, na próxima semana, juntávamos tudo e desenvolviámos a próxima etapa do projeto. O acompanhamento das tarefas foi feito por meio de levantamentos nessas reuniões, em que decidíamos o que precisava ser feito e quando. Dessa maneira, nos programamos construindo listas de tarefas com o uso do Tasks/Agenda (do ©Google) e as colocando no grupo do ©Whatsapp para ter uma recordação maior dos procedimentos.

-	**Dificultades encontradas / Soluções adotadas**

No decorrer do projeto, uma das principais dificuldades foi o tempo, pois cada um possui certos afazeres e compromissos que limitavam o número de encontros. A solução para isso foi desenvolver o bruto em um dia da semana, com uma quantidade maior de funcionalidades e, ao longo da semana, lapidando isso quando tivéssemos um tempo livre. 
O difícil acesso à informações construtivas, tendo em vista a quantidade massiva de informações repetitivas e por vezes enganosas, atrasou, de certa forma, o emprego de algumas soluções. Um exemplo simples foi o uso do resistor de pull-up no pino de dados do DHT11, em que o “datasheet” não especificava e que algumas fontes dizem ser desnecessário, mas que percebemos ser necessário em nossos testes. Dois exemplos mais complexos são o SSL para a encriptação e o uso do node SVG, com explicações muito semelhantes (por vezes iguais), mas as implementações relativamente diferentes. Para solucionar isso, foi necessário uma série de testes para cada coisa, o que ocupou certo tempo, mas permitiu um melhor entendimento sobre o problema em questão.

-	**Avaliação sobre o desenvolvimento do projeto**

É possível destacar um ponto positivo já na apresentação da proposta do trabalho, pois nela é exigido apenas que fosse desenvolvida uma aplicação IoT, sem limitações para o que desenvolver em cima desse tópico, assim, estimulando a criatividade dos integrantes para construir o que gostariamos com os recursos disponíveis. A necessidade de pesquisa sobre componentes a serem utilizados, tanto na parte de hardware quanto na parte de software, foi muito importante para aflorar um senso crítico maior sobre as informações encontradas na internet. E, também, será destacado a possibilidade de visualizar o progresso de cada passo do projeto por meio de testes práticos, tornando cada evolução mais visível e palpável.

Um ponto negativo em relação ao andar do projeto, além da disponibilidade individual de horários, como já comentado, foi a desigualdade do desempenho dos computadores utilizados por cada aluno, dificultando o acesso simultâneo às reuniões online do grupo e o desenvolvimento do software. De maneira geral, as dificuldades encontradas e os pontos negativos do projeto se limitam à problemas mais singulares, que, em condições normais, seriam menos acentuadas, mas que na circunstância atual possuem seu impacto.

## O QUE FOI SOLICITADO
**Projeto MeuTrecoNaInternet – Aplicação IoT**
- O objetivo deste trabalho é criar uma aplicação IoT. Esta aplicação consiste de:
	- Ao menos, 2 dispositivos IoT com medição e/ou atuação. Os dispositivos podem ser iguais ou não, mas devem estar em localizações geográficas distintas.
	- Uma interface de usuário com capacidade de visualização/controle, desenvolvida em STM32Monitor / NodeRed
- A aplicação desenvolvida deve ser coerente com os dispositivos aplicados. É desejável que a aplicação consiga extrair alguma informação relevante a partir da combinação dos dados dos dois dispositivos. O dispositivo IoT pode ser implementado usando as seguintes plataformas tecnológicas, ou combinações entre elas:
	- STM32 + PC, com link via serial ou bluetooth
	- Raspberry pi
	- ESP32
	- Arduino + interface wi-fi
	- Celular ou tablet (somente um dos dispositivos)
- Os sensores/atuadores a serem utilizados são os de interesse/disponibilidade do grupo. A aplicação embarcada pode ser desenvolvida em linguagem C, C++ ou python. Opcionalmente pode-se desenvolver uma aplicação para coletar, processar e/ou armazenar os dados provenientes dos dispositivos A comunicação será realizada através do protocolo MQTT. Os tópicos utilizados devem ser criados a partir do tópico iotTro, como utilizado no primeiro trabalho de mqtt. Deve-se utilizar uma conexão encriptada, de preferência com certificação. O broker a ser utilizado é de escolha do grupo de trabalho. Caso o broker da Fundação Liberato estiver disponível, seu uso será obrigatório;
- **Grupo de trabalho :**
	- O grupo de trabalho será composto por até 4 alunos, independente da turma. Recomenda-se o uso de plataformas de desenvolvimento, como o github. Caso isto seja uma opção do grupo, incluir o professor no projeto.
- **Prazos e entregas :**
	- 28 ago 2020 : Proposta
	- 07 nov 2020 : Desenvolvimento e apresentação
- **Avaliação**
	- Resultado funcional obtido, avaliado através da aplicação de interface com o usuário
	- Atendimento às especificações e prazos dados.
	- Criatividade na proposta e nas soluções utilizadas realização do projeto.
	- Qualidade da documentação entregue (organização, documentação codigos-fonte, clareza e objetividade das informações prestadas).
