/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file       main.c
  * @authors	Adriano André, Bruno Berwanger, Fabrício Lutz, Jordana Dutra.
  * @class		4411
  * @date		Agosto -> Setembro/ 2020
  * @company	Eletrônica - Fundação Liberato
  * @brief      Controle de Temperatura, Umidade do solo e Luz.
  ******************************************************************************
  * @attention
  * Código utilizado para o trabalho MeuTrecoNaInternet - MICROS II - FETLSVC
  * Pinagem atuadores: PC0->Cooler, PC1->Peltier
  * Pinagem  sensores: PA0->LDR,    PA1->DHT11,    PA4->YL-69
  *
  ******************************************************************************
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
#define SAMPLE  20								///< Número de amostras
#define PERCENT (100.0/(4095.0*10.0)) 			///< Cálculo da porcentagem a partir da resolução
#define SIZE_TX 100								///< Tamanho máximo do buffer TX da usart
#define SIZE_RX 29								///< Tamanho máximo do buffer RX da usart
#define LIGHT 	0								///< Para auxiliar no vetor de média
#define MOIST 	1								///< Para auxiliar no vetor de média
#define NAME 	"GAMA"							///< Nome da estufa
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float Temperature, Humidity, Light, Moisture;	///< Variáveis de cada sensor [Medições]
uint8_t Peltier_State, Cooler_State;			///< Variáveis para acompanhar o acionamento dos atuadores
volatile uint16_t measure[SAMPLE];				///< Vetor para armazenar as medições do ADC
uint16_t average[2];							///< Média aritmética das medições do ADC
char bufferRX[SIZE_RX];							///< Buffer de recebimento da USART
char bufferTX[SIZE_TX];							///< Buffer de envio da USART
float parameter[6]={0,50,0,100,0,100};			///< Vetor com os parâmetros ideias da estufa
enum PARAM{										///< Enumeração para uso do vetor parameter
	Temperature_Min,
	Temperature_Max,
	Moisture_Min,
	Moisture_Max,
	Lightness_Min,
	Lightness_Max,
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
  /* USER CODE BEGIN 2 */
  // ATIVAÇÃO DO ADC
  HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_1);								// Ativa o timer 3 como trigger do ADC
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*) measure, SAMPLE);				// Dispara a conversão circular do ADC

  // USART
  HAL_UART_Receive_DMA(&huart2, (uint8_t*) bufferRX, SIZE_RX);			// Ativa o recebimento de dados pela USART
  HAL_TIM_Base_Start_IT(&htim10);										// Ativa o timer 10 para leitura da temperatura/umidade e envia pela usart
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
	// Salva os valores da estrutura do DHT11
		DHT_GetData(&DHT11_Data);
		Temperature = DHT11_Data.Temperature;
		Humidity = DHT11_Data.Humidity;
		if(Temperature<parameter[Temperature_Min]){														//Acionar Peltier
			HAL_GPIO_WritePin(Acionamento_Peltier_GPIO_Port, Acionamento_Peltier_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Acionamento_Cooler_GPIO_Port, Acionamento_Cooler_Pin, GPIO_PIN_SET);
			Peltier_State = 1;
			Cooler_State = 1;
		}
		if((Temperature>=parameter[Temperature_Min])&&(Temperature<=parameter[Temperature_Max])){		// Desliga a Peltier e o Cooler
			HAL_GPIO_WritePin(Acionamento_Peltier_GPIO_Port, Acionamento_Peltier_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Acionamento_Cooler_GPIO_Port, Acionamento_Cooler_Pin, GPIO_PIN_RESET);
			Peltier_State = 0;
			Cooler_State = 0;
		}
		if(Temperature>parameter[Temperature_Max]){														// Acionar Cooler
			HAL_GPIO_WritePin(Acionamento_Peltier_GPIO_Port, Acionamento_Peltier_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Acionamento_Cooler_GPIO_Port, Acionamento_Cooler_Pin, GPIO_PIN_SET);
			Peltier_State = 0;
			Cooler_State = 1;
		}
	// Envio dos dados coletados em JSON Object
		sprintf(bufferTX, "{\"Name\": \"%s\", \"Temperature\": %2.1f, \"Light\": %2.1f, \"Moisture\": %2.1f, \"Humidity\": %2.1f}",NAME, Temperature, Light, Moisture, Humidity);
		HAL_UART_Transmit_DMA(&huart2, (uint8_t*) bufferTX, strlen(bufferTX));

}
/**
  * @brief  Callback de conversão completa do ADC, faz a média dos valores e calcula a porcentagem.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	// Zera as posições do vetor de média
		average[LIGHT] = 0; average[MOIST] = 0;
	// Preencher o vetor com a soma de todas as medidas feitas para cada sensor
		for(int i=0;i<SAMPLE;i++)
			{
			if((i%2)==0)average[LIGHT] += measure[i];
			else average[MOIST] += measure[i];
			}
	// Retorna a porcentagem de cada sensor
		Light = average[LIGHT]*PERCENT;
		Moisture = average[MOIST]*PERCENT;
}
/**
  * @brief  Callback de recepção da USART, os dados do usuário são recebidos, divididos e convertidos nos valores de parâmetros.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	__HAL_UART_FLUSH_DRREGISTER(huart);				// Limpa o buffer de recepção para evitar overrun
	int i=0;										// Variável auxiliar de contagem do vetor parameter
	char* pieces = strtok(bufferRX, " ");			// Cria um ponteiro Char que aponta para o primeiro "pedaço" da string até o caracter " "
	while (pieces != NULL){							// Enquanto o ponteiro não indicar o fim da string
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
