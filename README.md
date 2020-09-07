# MTI_EstufaAutomatica
### Autores: 
- Adriano André da Silva ([GitHub](https://github.com/Adriano-Andre/))
- Bruno Gabriel Berwanger ([GitHub](https://github.com/BrunoGabrielBerwanger/))
- Fabrício Pereira Lutz ([GitHub](https://github.com/FabricioLutz/))
- Jordana Pedroso Dutra ([GitHub](https://github.com/DutraJordana/))
### Professor: Marcos Zuccolotto ([GitHub](https://github.com/Zucco1997/)), Disciplina: Micros II, Curso Técnico de Eletrônica - 4° ANO
### [Fundação Escola Técnica Liberato Salzano Vieira da Cunha](https://liberato.com.br)

## PROJETO
-	O projeto Estufa Automática tem por objetivo desenvolver três estufas, medindo e atuando sobre a temperatura, luminosidade e umidade do solo. Essa implementação fará uso do protocolo MQTT, tornando as estufas em objetos virtuais para IoT, o que permitirá um controle à distância das estufas (que estarão colocadas em duas cidades) e,além disso, permitirá uma análise de consumo por parte de cada estufa à partir do consumo medido nas. Dessa maneira, cria-se a possibilidade de identificar falhas na aplicação, qual estufa obteve menores gastos e criar alertas em caso de defeitos.

## O QUE FOI SOLICITADO
**Projeto MeuTrecoNaInternet – Aplicação IoT**
- O objetivo deste trabalho é criar uma aplicação IoT. Esta aplicação consiste de:
	- Ao menos, 2 dispositivos IoT com medição e/ou atuação. Os dispositivos podem ser iguais ou não, mas devem estar em localizações geográficas distintas.
	- Uma interface de usuário com capacidade de visualização/controle, desenvolvida em STM32Monitor / NodeRed
- A aplicação desenvolvida deve ser coerente com os dispositivos aplicados. É desejável que a aplicação consiga extrair alguma informação relevante a partir da combinação dos dados dos dois dispositivos. O dispositivo IoT pode ser implementando usando as seguintes plataformas tecnológicas, ou combinações entre elas:
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
	- 01 out 2020 : Desnvolvimento
- **Avaliação**
	- Resultado funcional obtido, avaliado através da aplicação de interface com o usuário
	- Atendimento às especificações e prazos dados.
	- Criatividade na proposta e nas soluções utilizadas realização do projeto.
	- Qualidade da documentação entregue (organização, documentação codigos-fonte, clareza e objetividade das informações prestadas).
