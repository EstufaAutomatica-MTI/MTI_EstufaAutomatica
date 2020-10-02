/* USER CODE BEGIN Header */
/**
  ******************************************************************************************************************
  * @file       main.c
  * @authors	Adriano André, Bruno Berwanger, Fabrício Lutz, Jordana Dutra.
  * @class		4411
  * @date		Agosto -> Setembro -> Outubro/ 2020
  * @company	Eletrônica - Fundação Liberato
  * @brief      Controle de Temperatura, Umidade do solo e Luz.
  ******************************************************************************************************************
  * @attention
  * Código utilizado para o trabalho MeuTrecoNaInternet - MICROS II - FETLSVC
  * Pinagem atuadores: PC0->WaterPump, PC1->PeltierPlate, PC2->CoolerFreeze, PC3->CoolerPeltier, PC6->LedVermelho, PC8->LedVerde, PC9->LedAzul
  * Pinagem sensores: PA0->LDR, PA1->DHT11, PA4->YL-69, PA7->ACS712
  * Pinagem de falha: PB7->Shutdown
  *
  ******************************************************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DHT.h"								///< Biblioteca para o sensor DHT11 - Desenvolvida por: Controllers Tech (controllerstech.com)
#include "string.h"								///< Biblioteca básica
#include "stdio.h"								///< Biblioteca básica
#include "stdlib.h"								///< Biblioteca básica
#include "math.h"								///< Biblioteca básica
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
DHT_DataTypedef DHT11_Data;						///< Cria a estrutura para o sensor
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NAME 	"ALFA"							///< Nome da estufa (ALFA, BETA, GAMA)
#define VOLTAGE 12.00							///< Tensão elétrica usada para acionamento dos atuadores (Volts)
#define I_MAX   4.00							///< Máxima corrente elétrica (Amperes)
#define SAMPLE  20								///< Número de amostras a serem analisadas pelo ADC na medição de Luz e Umidade do Solo
#define PERCENT (100.0/(4095.0*10.0)) 			///< Cálculo da porcentagem para medição de Luz e Umidade do Solo
#define SIZE_TX 150								///< Tamanho máximo do buffer TX da usart
#define SIZE_RX 31								///< Tamanho máximo do buffer RX da usart
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float Temperature, Humidity, Light, Moisture, Current;	///< Variáveis de cada sensor
float Power;											///< Variáveis de consumo elétrico
uint8_t StateOf[4];										///< Vetor para acompanhar estado de alguns atuadores
volatile uint16_t measure_ADC1[SAMPLE];					///< Vetor para armazenar as leituras do YL-69 e LDR, no ADC1
volatile uint16_t measure_ADC2[SAMPLE];					///< Vetor para armazenar as leituras do ACS712, no ADC2
uint16_t average[3];									///< Média aritmética das medições do ADC1 e ADC2 (LDR, YL-69, ACS712)
char bufferRX[SIZE_RX];									///< Buffer de recebimento da USART
char bufferTX[SIZE_TX];									///< Buffer de envio da USART
float parameter[7]={0,50,0,100,255,255,255};			///< Vetor com os parâmetros ideais da estufa (TemperaturaMinima,TemperaturaMaxima,UmidadeDoSoloMinima, UmidadeDoSoloMaxima, LuzVermelha, LuzVerde, LuzAzul)
enum AUX{	 											///< Enumeração para uso auxiliar dos vetores
	Temperature_Min,									// Variáveis para vetor parameter[]
	Temperature_Max,									// -
	Moisture_Min,										// -
	Moisture_Max,										// -
	LightRed,											// -
	LightGreen,											// -
	LightBlue,											// -
	CoolerPeltier=0,									// Variáveis para vetor StateOf[]
	PeltierPlate,										// -
	CoolerFreeze,										// -
	ShutDown,											// -
	LDR=0,												// Variáveis paravetor average[]
	YL_69,												// -
	ACS712												// -
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_TIM10_Init();
  MX_TIM2_Init();
  MX_TIM11_Init();
  MX_ADC2_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
  // ATIVAÇÃO DO ADC1 E ADC2
  HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_1);								// Ativa o timer 2 como trigger do ADC1
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*) measure_ADC1, SAMPLE);			// Dispara a conversão DMA circular do ADC1

  HAL_TIM_Base_Start(&htim8);											// Ativa o timer 8 como trigger do ADC2
  HAL_ADC_Start_DMA(&hadc2, (uint32_t*) measure_ADC2, SAMPLE);			// Dispara a conversão DMA circular do ADC2

  // ATIVAÇÃO DA USART2 PARA RECEBIMENTO DE DADOS
  HAL_UART_Receive_DMA(&huart2, (uint8_t*) bufferRX, SIZE_RX);			// Ativa o recebimento de dados pela USART

  // ATIVAÇÃO DO TIM11 PARA ATIVAÇÃO DOS ATUADORES E ENVIO DE DADOS
  HAL_TIM_Base_Start_IT(&htim11);										// Ativa o timer 11 para análisar os estados da estufa e enviar os dados lidos

  // ATIVAÇÃO DO TIM10 PARA LEITURAS DO DHT11
  HAL_TIM_Base_Start_IT(&htim10);										// Ativa o timer 10 para leitura da temperatura/umidade do DHT11

  // PWM PARA FITA DE LED
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);								// Dispara a geração do sinal PWM - TIMER 3
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);								// Dispara a geração do sinal PWM - TIMER 3
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);								// Dispara a geração do sinal PWM - TIMER 3

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Callback dos timers 10 e 11 para medição dos dados coletados do DHT11, análise de estados e envio de todas as informações dos sensores.
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// GUARDA OS VALORES DA ESTRUTURA DO DHT11 PÓS SALVAMENTO
	if(htim->Instance==TIM10)
	{											//
		DHT_GetData(&DHT11_Data);				// Pega os valores do sensor
		Temperature = DHT11_Data.Temperature;	// Guarda a temperatura na variável flutuante
		Humidity = DHT11_Data.Humidity;			// Guarda a umidade do ar na variável flutuante
	}											// Fim do estouro do TIM10

	// AN�?LISE DOS ESTADOS DA ESTUFA E ENVIO DOS DADOS COLETADOS
	if((htim->Instance==TIM11)&&(StateOf[ShutDown]==0))	// Se o timer 11 estourou e o Shutdown está desativado
	{
		// ENVIO DOS DADOS COLETADOS EM FORMATO JSON OBJECT PARA O NODE RED
		sprintf(bufferTX, "{\"Name\": \"%s\", \"Temperature\": %2.1f, \"Light\": %2.1f, \"Moisture\": %2.1f, \"Humidity\": %2.1f, \"Power\": %2.1f}", NAME, Temperature, Light, Moisture, Humidity, Power);
		HAL_UART_Transmit_DMA(&huart2, (uint8_t*) bufferTX, strlen(bufferTX));				// Envia a string bufferTX pela função Transmit DMA

		// ESTADOS DE TEMPERATURA COM HISTERESE
		if(Temperature<=(parameter[Temperature_Min]+1)&&(StateOf[CoolerFreeze]==1))			// Se o resfriamento está ativado, desliga somente ao alcançar a temperatura mínimo
		{																					//
			HAL_GPIO_WritePin(PeltierPlate_GPIO_Port, PeltierPlate_Pin,   GPIO_PIN_RESET);	// Desliga o atuador
			HAL_GPIO_WritePin(CoolerPeltier_GPIO_Port, CoolerPeltier_Pin, GPIO_PIN_RESET);	// Desliga o atuador
			HAL_GPIO_WritePin(CoolerFreeze_GPIO_Port, CoolerFreeze_Pin,   GPIO_PIN_RESET);	// Desliga o atuador
			StateOf[PeltierPlate]  = 0;														// Guarda o estado atual
			StateOf[CoolerPeltier] = 0;														// Guarda o estado atual
			StateOf[CoolerFreeze]  = 0;														// Guarda o estado atual
		}																					//
		else if((Temperature<parameter[Temperature_Min])&&(StateOf[CoolerFreeze]==0))		// Se o resfriamento está desativado e a temperatura está abaixo do mínimo
		{																					//
			HAL_GPIO_WritePin(PeltierPlate_GPIO_Port, PeltierPlate_Pin,   GPIO_PIN_SET);	// Liga o atuador
			HAL_GPIO_WritePin(CoolerPeltier_GPIO_Port, CoolerPeltier_Pin, GPIO_PIN_SET);	// Liga o atuador
			HAL_GPIO_WritePin(CoolerFreeze_GPIO_Port, CoolerFreeze_Pin,   GPIO_PIN_RESET);	// Desliga o atuador
			StateOf[PeltierPlate]  = 1;														// Guarda o estado atual
			StateOf[CoolerPeltier] = 1;														// Guarda o estado atual
			StateOf[CoolerFreeze]  = 0;														// Guarda o estado atual
		}																					//
		else if(Temperature>=(parameter[Temperature_Max]-1)&&(StateOf[PeltierPlate]==1))	// Se o aquecimento está ativado, desliga somente ao alcançar a temperatura máxima
		{																					//
			HAL_GPIO_WritePin(PeltierPlate_GPIO_Port, PeltierPlate_Pin,   GPIO_PIN_RESET);	// Desliga o atuador
			HAL_GPIO_WritePin(CoolerPeltier_GPIO_Port, CoolerPeltier_Pin, GPIO_PIN_RESET);	// Desliga o atuador
			HAL_GPIO_WritePin(CoolerFreeze_GPIO_Port, CoolerFreeze_Pin,   GPIO_PIN_RESET);	// Desliga o atuador
			StateOf[PeltierPlate]  = 0;														// Guarda o estado atual
			StateOf[CoolerPeltier] = 0;														// Guarda o estado atual
			StateOf[CoolerFreeze]  = 0;														// Guarda o estado atual
		}																					//
		else if((Temperature>parameter[Temperature_Max])&&(StateOf[PeltierPlate]==0))		// Se o aquecimento está desativado e a temperatura está acima do máximo
		{																					//
			HAL_GPIO_WritePin(PeltierPlate_GPIO_Port, PeltierPlate_Pin,   GPIO_PIN_RESET);	// Desliga o atuador
			HAL_GPIO_WritePin(CoolerPeltier_GPIO_Port, CoolerPeltier_Pin, GPIO_PIN_SET);	// Liga o atuador
			HAL_GPIO_WritePin(CoolerFreeze_GPIO_Port, CoolerFreeze_Pin,   GPIO_PIN_SET);	// Liga o atuador
			StateOf[PeltierPlate]  = 0;														// Guarda o estado atual
			StateOf[CoolerPeltier] = 1;														// Guarda o estado atual
			StateOf[CoolerFreeze]  = 1;														// Guarda o estado atual
		}																					//
		// ESTADOS DA UMIDADE DO SOLO COM HISTERESE
		if(Moisture<parameter[Moisture_Min])												// Se a umidade está menor que o mínimo
		{																					//
			HAL_GPIO_WritePin(WaterPump_GPIO_Port, WaterPump_Pin, GPIO_PIN_SET);			// Acionar modo de irrigação
		}																					//
		if(Moisture>=parameter[Moisture_Max])												// Se a umidade está acima do máximo
		{																					//
			HAL_GPIO_WritePin(WaterPump_GPIO_Port, WaterPump_Pin, GPIO_PIN_RESET);			// Desliga o modo de irrigação
		}																					//
		// ESTADOS DA LUZ
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,parameter[LightRed]);					// Corrige a intensidade da cor
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,parameter[LightGreen]);					// Corrige a intensidade da cor
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,parameter[LightBlue]);					// Corrige a intensidade da cor
	}																						// Fim do estouro do TIM11
}
/**
  * @brief  Callback de conversão completa do ADC1 e o ADC2, faz o processamento dos valores obtidos.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	// GUARDA OS VALORES DOS SENSORES LDR E YL-69
	if(hadc->Instance==ADC1)								//
	{														//
		average[LDR] = 0; average[YL_69] = 0;				// Zera as posições dos vetors de média
		for(int i=0;i<SAMPLE;i++)							// Laço para guardar as medidas
			{												//
				if((i%2)==0)average[LDR] += measure_ADC1[i];// Os valores pares são o LDR, soma todas as medições e salva no vetor
				else average[YL_69] += measure_ADC1[i];		// Os valores ímpares são o YL-69, soma todas as medições e salva no vetor
			}												//
		Light = average[LDR]*PERCENT;						// Retorna a porcentagem do valor medido de cada sensor
		Moisture = average[YL_69]*PERCENT;					// Retorna a porcentagem do valor medido de cada sensor
	}														// Fim da conversão completa do ADC1
	// GUARDA OS VALORES DO SENSOR DE CORRENTE
	if(hadc->Instance==ADC2)								//
	{														//
		average[ACS712] = 0;								// Zera as posições do vetor de média
		for(int i=0;i<SAMPLE;i++)							// Laço para guardar as medidas
			{												//
			average[ACS712] += measure_ADC2[i];				// Soma todas as medições e salva no vetor
			}
		//Cálculo da corrente elétrica
		Current = abs((3103.0-(average[ACS712]/20.0))/4096.0*(3.3/0.1));		            // Calcula a corrente elétrica da medição média do sensor de corrente
		//Controle de falhas
		if(Current>I_MAX)// Se a corrente atual ultrapassa o valor de todos os atuadores ligados o circuito entra em proteção e é desligado
		{
			HAL_GPIO_WritePin(Shutdown_GPIO_Port, Shutdown_Pin, GPIO_PIN_SET);				// Ativa o desligamento forçado dos atuadores
			HAL_GPIO_WritePin(PeltierPlate_GPIO_Port, PeltierPlate_Pin,   GPIO_PIN_RESET);	// Desativação do atuador
			HAL_GPIO_WritePin(CoolerPeltier_GPIO_Port, CoolerPeltier_Pin, GPIO_PIN_RESET);	// Desativação do atuador
			HAL_GPIO_WritePin(CoolerFreeze_GPIO_Port, CoolerFreeze_Pin,   GPIO_PIN_RESET);	// Desativação do atuador
			HAL_GPIO_WritePin(WaterPump_GPIO_Port,    WaterPump_Pin,      GPIO_PIN_RESET);	// Desativação do atuador
		    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);										// Desliga a geração do sinal PWM - TIMER 3
		    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);										// Desliga a geração do sinal PWM - TIMER 3
		    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);										// Desliga a geração do sinal PWM - TIMER 3
			StateOf[PeltierPlate]  = 0;	// Guarda estado atual
			StateOf[CoolerPeltier] = 0;	// Guarda estado atual
			StateOf[CoolerFreeze]  = 0;	// Guarda estado atual
		}
		//Cálculo da potência elétrica
		Power = VOLTAGE * Current;							// Calcula a potência elétrica do instante
	} 														// Fim da conversão completa do ADC2
}
/**
  * @brief  Callback de recepção da USART, os dados do usuário são recebidos, divididos e convertidos nos valores de parâmetros.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	__HAL_UART_FLUSH_DRREGISTER(huart);				// Limpa o buffer de recepção para evitar overrun
	// DESATIVAÇÃO DOS ATUADORES PARA EVITAR CONFLITOS COM POSSIVEIS NOVOS VALORES
	HAL_GPIO_WritePin(PeltierPlate_GPIO_Port, PeltierPlate_Pin,   GPIO_PIN_RESET);	// Desativação do atuador
	HAL_GPIO_WritePin(CoolerPeltier_GPIO_Port, CoolerPeltier_Pin, GPIO_PIN_RESET);	// Desativação do atuador
	HAL_GPIO_WritePin(CoolerFreeze_GPIO_Port, CoolerFreeze_Pin,   GPIO_PIN_RESET);	// Desativação do atuador
	HAL_GPIO_WritePin(WaterPump_GPIO_Port,    WaterPump_Pin,      GPIO_PIN_RESET);	// Desativação do atuador
	StateOf[PeltierPlate]  = 0;	// Guarda estado atual
	StateOf[CoolerPeltier] = 0;	// Guarda estado atual
	StateOf[CoolerFreeze]  = 0;	// Guarda estado atual
	// SEPARAÇÃO DA STRING E CONVERSÃO PARA FLOAT - Código explicado por CodeVault (youtube.com/channel/UC6qj_bPq6tQ6hLwOBpBQ42Q)
	int i=0;										// Variável auxiliar de contagem do vetor parameter
	char* pieces = strtok(bufferRX, " ");			// Cria um ponteiro Char que aponta para o primeiro "pedaço" da string até o caracter " "
	while (pieces != NULL)							// Enquanto o ponteiro não indicar o fim da string
	{												//
		parameter[i] = atof(pieces);				// Converte o pedaço da string para float
		pieces = strtok(NULL, " ");					// Indica que permanece na mesma string e, então, segue para o próximo pedaço dela
		i++;										// Segue para o próximo parâmetro ser convertido
	}
	if((StateOf[ShutDown] == 1)&&(parameter[0] == 99.9))						// Se o desligamento forçado foi ativado e o código de reativação foi recebido
	{
		HAL_GPIO_WritePin(Shutdown_GPIO_Port, Shutdown_Pin, GPIO_PIN_RESET); 	// Desativa o desligamento forçado dos atuadores
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);								// Dispara a geração do sinal PWM - TIMER 3
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);								// Dispara a geração do sinal PWM - TIMER 3
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);								// Dispara a geração do sinal PWM - TIMER 3
		StateOf[ShutDown] = 0;
		parameter[0] = 0.0;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
