/* USER CODE BEGIN Header */
/**
  ******************************************************************************************************************
  * @file       main.c
  * @authors	Adriano André, Bruno Berwanger, Fabrício Lutz, Jordana Dutra.
  * @class		4411
  * @date		Agosto -> Setembro/ 2020
  * @company	Eletrônica - Fundação Liberato
  * @brief      Controle de Temperatura, Umidade do solo e Luz.
  ******************************************************************************************************************
  * @attention
  * Código utilizado para o trabalho MeuTrecoNaInternet - MICROS II - FETLSVC
  * Pinagem atuadores: PC0->WaterPump, PC1->PeltierPlate, PC2->CoolerFreeze, PC3->CoolerPeltier, PB9->LedStrip
  * Pinagem  sensores: PA0->LDR, PA1->DHT11, PA4->YL-69
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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
DHT_DataTypedef DHT11_Data;						///< Cria a estrutura para o sensor
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NAME 	"GAMA"							///< Nome da estufa (ALFA, BETA, GAMA)
#define N_ACT   5								///< Número de dispositivos atuadores da estufa
#define SAMPLE  20								///< Número de amostras a serem analisadas pelo ADC na medição de Luz e Umidade do Solo
#define PERCENT (100.0/(4095.0*10.0)) 			///< Cálculo da porcentagem para medição de Luz e Umidade do Solo
#define SIZE_TX 100								///< Tamanho máximo do buffer TX da usart
#define SIZE_RX 29								///< Tamanho máximo do buffer RX da usart
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float Temperature, Humidity, Light, Moisture;	///< Variáveis de cada sensor [Medições]
uint8_t StateOf[N_ACT];							///< Vetor para acompanhar estado de cada atuador
volatile uint16_t measure[SAMPLE];				///< Vetor para armazenar as medições do ADC
uint16_t average[2];							///< Média aritmética das medições do ADC
char bufferRX[SIZE_RX];							///< Buffer de recebimento da USART
char bufferTX[SIZE_TX];							///< Buffer de envio da USART
float parameter[6]={0,50,0,100,0,100};			///< Vetor com os parâmetros ideias da estufa
enum PARAM{	 									///< Enumeração para uso auxiliar dos vetores
	Temperature_Min,
	Temperature_Max,
	Moisture_Min,
	Moisture_Max,
	Lightness_Min,
	Lightness_Max,
	CoolerPeltier=0,
	PeltierPlate,
	CoolerFreeze,
	LedStrip,
	WaterPump,
	LDR=0,
	YL_69
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
  /* USER CODE BEGIN 2 */
  // ATIVAÇÃO DO ADC
  HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_1);								// Ativa o timer 3 como trigger do ADC
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*) measure, SAMPLE);				// Dispara a conversão circular do ADC

  // USART
  HAL_UART_Receive_DMA(&huart2, (uint8_t*) bufferRX, SIZE_RX);			// Ativa o recebimento de dados pela USART
  HAL_TIM_Base_Start_IT(&htim10);										// Ativa o timer 10 para leitura da temperatura/umidade e envia pela usart
  HAL_TIM_Base_Start_IT(&htim11);
  // PWM
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);								// Dispara a geração do sinal PWM - TIMER 2
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
  * @brief  Callback do timer 10 para medição dos dados coletados do DHT11 e envio de todas as informações dos sensores.
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM10){
	// Salva os valores da estrutura do DHT11
		DHT_GetData(&DHT11_Data);
		Temperature = DHT11_Data.Temperature;
		Humidity = DHT11_Data.Humidity;
	}

	if(htim->Instance==TIM11){
	// Análisa os estados da temperatura
		if(Temperature<=(parameter[Temperature_Min]+1)&&(StateOf[CoolerFreeze]==1))
		{
			HAL_GPIO_WritePin(PeltierPlate_GPIO_Port, PeltierPlate_Pin,   GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CoolerPeltier_GPIO_Port, CoolerPeltier_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CoolerFreeze_GPIO_Port, CoolerFreeze_Pin,   GPIO_PIN_RESET);
			StateOf[PeltierPlate]  = 0;
			StateOf[CoolerPeltier] = 0;
			StateOf[CoolerFreeze]  = 0;
		}
		else if((Temperature<parameter[Temperature_Min])&&(StateOf[CoolerFreeze]==0))
		{
			HAL_GPIO_WritePin(PeltierPlate_GPIO_Port, PeltierPlate_Pin,   GPIO_PIN_SET);
			HAL_GPIO_WritePin(CoolerPeltier_GPIO_Port, CoolerPeltier_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CoolerFreeze_GPIO_Port, CoolerFreeze_Pin,   GPIO_PIN_RESET);
			StateOf[PeltierPlate]  = 1;
			StateOf[CoolerPeltier] = 1;
			StateOf[CoolerFreeze]  = 0;
		}
		else if(Temperature>=(parameter[Temperature_Max]-1)&&(StateOf[PeltierPlate]==1))
		{
			HAL_GPIO_WritePin(PeltierPlate_GPIO_Port, PeltierPlate_Pin,   GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CoolerPeltier_GPIO_Port, CoolerPeltier_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CoolerFreeze_GPIO_Port, CoolerFreeze_Pin,   GPIO_PIN_RESET);
			StateOf[PeltierPlate]  = 0;
			StateOf[CoolerPeltier] = 0;
			StateOf[CoolerFreeze]  = 0;
		}
		else if((Temperature>parameter[Temperature_Max])&&(StateOf[PeltierPlate]==0))
		{
			HAL_GPIO_WritePin(PeltierPlate_GPIO_Port, PeltierPlate_Pin,   GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CoolerPeltier_GPIO_Port, CoolerPeltier_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(CoolerFreeze_GPIO_Port, CoolerFreeze_Pin,   GPIO_PIN_SET);
			StateOf[PeltierPlate]  = 0;
			StateOf[CoolerPeltier] = 1;
			StateOf[CoolerFreeze]  = 0;
		}

		if(Moisture<parameter[Moisture_Min])
		{
			HAL_GPIO_WritePin(WaterPump_GPIO_Port, WaterPump_Pin, GPIO_PIN_SET);					// Acionar modo de irrigação
		}

		if(Moisture>=parameter[Moisture_Max])
		{
			HAL_GPIO_WritePin(WaterPump_GPIO_Port, WaterPump_Pin, GPIO_PIN_RESET);					// Desliga o modo de irrigação
		}
	// Envio dos dados coletados em JSON Object
		sprintf(bufferTX, "{\"Name\": \"%s\", \"Temperature\": %2.1f, \"Light\": %2.1f, \"Moisture\": %2.1f, \"Humidity\": %2.1f}",NAME, Temperature, Light, Moisture, Humidity);
		HAL_UART_Transmit_DMA(&huart2, (uint8_t*) bufferTX, strlen(bufferTX));

	}
}
/**
  * @brief  Callback de conversão completa do ADC, faz a média dos valores e calcula a porcentagem.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	// Zera as posições do vetor de média
		average[LDR] = 0; average[YL_69] = 0;
	// Preencher o vetor com a soma de todas as medidas feitas para cada sensor
		for(int i=0;i<SAMPLE;i++)
			{
			if((i%2)==0)average[LDR] += measure[i];
			else average[YL_69] += measure[i];
			}
	// Retorna a porcentagem de cada sensor
		Light = average[LDR]*PERCENT;
		Moisture = average[YL_69]*PERCENT;
}
/**
  * @brief  Callback de recepção da USART, os dados do usuário são recebidos, divididos e convertidos nos valores de parâmetros.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	__HAL_UART_FLUSH_DRREGISTER(huart);				// Limpa o buffer de recepção para evitar overrun
	int i=0;										// Variável auxiliar de contagem do vetor parameter
	// Código explicado por CodeVault (youtube.com/channel/UC6qj_bPq6tQ6hLwOBpBQ42Q)
	char* pieces = strtok(bufferRX, " ");			// Cria um ponteiro Char que aponta para o primeiro "pedaço" da string até o caracter " "
	while (pieces != NULL)							// Enquanto o ponteiro não indicar o fim da string
	{
		parameter[i] = atof(pieces);				// Converte o pedaço da string para float
		pieces = strtok(NULL, " ");					// Indica que permanece na mesma string e, então, segue para o próximo pedaço dela
		i++;										// Segue para o próximo parâmetro ser convertido
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
