## STM32でのシリアル通信の方法

このドキュメントでは、STM32マイコンを使用してPCとシリアル通信を行う手順を丁寧に解説します。STM32側の設定、Python側での通信プログラムも紹介し、双方が安定した通信を行う方法を説明します。

### 1. STM32のUART設定

#### 1.1 初期設定

STM32でシリアル通信を行うには、UART (Universal Asynchronous Receiver Transmitter) を設定します。STM32CubeMXを使用して以下の設定を行います。

1. **ペリフェラル設定**  
   STM32CubeMXでプロジェクトを開き、`USART2`（例）などのUARTを選択して有効化します。

2. **パラメータ設定**  
   - **ボーレート**: 115200（必要に応じて変更可能）
   - **データ長**: 8ビット
   - **パリティ**: None
   - **ストップビット**: 1
   - **モード**: `Tx/Rx`（送受信）

3. **クロック設定**  
   クロックツリーを設定して、UARTのクロックが適切に供給されていることを確認します。

4. **コードの生成**  
   設定が完了したら、プロジェクトのコードを生成します。

#### 1.2 UART送受信コードの実装

`main.c` などのプログラムファイルで、UARTの送受信コードを追加します。

以下は、データを送受信するコードの例です。この例では、送信データの先頭にヘッダーとして`0xA5 0xA5`を追加し、受信時にはヘッダーが正しいかを確認しています。

```c
#include "main.h"
#include <string.h>

UART_HandleTypeDef huart2;
uint8_t rx_buffer[50];  // 受信バッファ

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    uint8_t header[] = {0xA5, 0xA5};
    char tx_data[50];
    int speedFR = 90, speedFL = 50, speedBR = 10;

    while (1) {
        // データの送信
        snprintf(tx_data, sizeof(tx_data), "%d %d %d\n", speedFR, speedFL, speedBR);
        HAL_UART_Transmit(&huart2, header, sizeof(header), HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart2, (uint8_t *)tx_data, strlen(tx_data), HAL_MAX_DELAY);

        // データの受信
        if (HAL_UART_Receive(&huart2, rx_buffer, sizeof(rx_buffer), 100) == HAL_OK) {
            if (rx_buffer[0] == 0xA5 && rx_buffer[1] == 0xA5) {
                // ヘッダーが一致する場合、データの処理
                printf("Received data: %s\n", &rx_buffer[2]);
            }
        }
        HAL_Delay(500);  // 少し待機
    }
}

// システムクロック、GPIO、USARTの初期化関数（略）
```

### 2. PythonでのPC側シリアル通信プログラム

PC側では、Pythonを使ってSTM32とのシリアル通信を行います。以下は、Pythonの`pyserial`ライブラリを使用したサンプルコードです。

#### 2.1 Pythonのコード

以下のPythonコードは、STM32に対して速度データを送信し、受信データがヘッダー`0xA5 0xA5`で始まっているかを確認します。

```python
import serial
import time

# シリアルポートの設定
port = "/dev/ttyACM0"  # Linuxの場合（例: "/dev/ttyUSB0"）
baudrate = 115200

try:
    # シリアル接続の初期化
    ser = serial.Serial(port, baudrate, timeout=0.01)
    print("Connected to", port)

    while True:
        # 3つのモーター速度データを送信
        speedFR = 90
        speedFL = 50
        speedBR = 10
        send_data = bytearray([0xA5, 0xA5]) + f"{speedFR} {speedFL} {speedBR}\n".encode('utf-8')
        ser.write(send_data)  # STM32に送信

        # バイト列で受信してヘッダーを確認
        if ser.in_waiting > 0:
            received_data = ser.read_until(b'\n')  # バイト列で受信
            if received_data.startswith(b'\xA5\xA5'):
                # ヘッダーを除いたデータ部分をデコードして表示
                actual_data = received_data[2:].decode('utf-8', errors='ignore').strip()
                print("Received from STM32:", actual_data)

except serial.SerialException as e:
    print("Serial error:", e)

finally:
    if ser.is_open:
        ser.close()
        print("Connection closed")
```

### 3. 説明

- **送信側（STM32）**: 送信データの先頭にヘッダーとして`0xA5 0xA5`をつけます。データは速度情報をスペースで区切った形式にしています。
- **受信側（Python）**: 受信データにヘッダーが含まれていることを確認し、ヘッダーが正しい場合のみデータを出力します。

### 4.サンプルコード
マイコン
```c
 #include "main.h"
#include "motor_driver.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim13;
TIM_HandleTypeDef htim12;
TIM_HandleTypeDef htim14;

UART_HandleTypeDef huart2;

/* モータードライバのインスタンス */
MotorDriver motorFR, motorFL, motorBR;

uint8_t rx_buffer[20];  // 受信バッファ

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM12_Init(void);
static void MX_TIM13_Init(void);
static void MX_TIM14_Init(void);

void handle_received_data(const char* data);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_TIM1_Init();
    MX_TIM12_Init();
    MX_TIM13_Init();
    MX_TIM14_Init();

    /* モーターの初期化 */
    MotorDriver_Init(&motorFR, &htim12, TIM_CHANNEL_1, &htim12, TIM_CHANNEL_2);
    MotorDriver_Init(&motorFL, &htim1, TIM_CHANNEL_1, &htim1, TIM_CHANNEL_4);
    MotorDriver_Init(&motorBR, &htim13, TIM_CHANNEL_1, &htim14, TIM_CHANNEL_1);

    while (1)
    {
        // データを受信
        if (HAL_UART_Receive(&huart2, rx_buffer, sizeof(rx_buffer), HAL_MAX_DELAY) == HAL_OK)
        {
            // ヘッダー確認
            if (rx_buffer[0] == 0xA5 && rx_buffer[1] == 0xA5)
            {
                // 受信した速度データの読み込み
                int speedFR, speedFL, speedBR;
                sscanf((char*)&rx_buffer[2], "%d %d %d", &speedFR, &speedFL, &speedBR);

                // モーター速度設定
                MotorDriver_setSpeed(&motorFR, speedFR);
                MotorDriver_setSpeed(&motorFL, speedFL);
                MotorDriver_setSpeed(&motorBR, speedBR);

                // 受信データをそのまま送り返す（ヘッダー含む）
                HAL_UART_Transmit(&huart2, rx_buffer, sizeof(rx_buffer), HAL_MAX_DELAY);
            }
        }
    }
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

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM12 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM12_Init(void)
{

  /* USER CODE BEGIN TIM12_Init 0 */

  /* USER CODE END TIM12_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM12_Init 1 */

  /* USER CODE END TIM12_Init 1 */
  htim12.Instance = TIM12;
  htim12.Init.Prescaler = 0;
  htim12.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim12.Init.Period = 65535;
  htim12.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim12.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim12) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM12_Init 2 */

  /* USER CODE END TIM12_Init 2 */
  HAL_TIM_MspPostInit(&htim12);

}

/**
  * @brief TIM13 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM13_Init(void)
{

  /* USER CODE BEGIN TIM13_Init 0 */

  /* USER CODE END TIM13_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM13_Init 1 */

  /* USER CODE END TIM13_Init 1 */
  htim13.Instance = TIM13;
  htim13.Init.Prescaler = 0;
  htim13.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim13.Init.Period = 65535;
  htim13.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim13.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim13) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim13) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim13, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM13_Init 2 */

  /* USER CODE END TIM13_Init 2 */
  HAL_TIM_MspPostInit(&htim13);

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 0;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 65535;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */
  HAL_TIM_MspPostInit(&htim14);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
```

Python
```py
import serial
import time

# シリアルポートの設定
port = "/dev/ttyACM0"  # Linuxの場合（例: "/dev/ttyUSB0"）
baudrate = 115200

try:
    # シリアル接続の初期化
    ser = serial.Serial(port, baudrate, timeout=0.01)
    print("Connected to", port)

    while True:
        # 3つのモーター速度データを送信
        speedFR = 90
        speedFL = 50
        speedBR = 10
        send_data = bytearray([0xA5, 0xA5]) + f"{speedFR} {speedFL} {speedBR}\n".encode('utf-8')
        ser.write(send_data)  # STM32に送信

        # バイト列で受信してヘッダーを確認
        if ser.in_waiting > 0:
            received_data = ser.read_until(b'\n')  # バイト列で受信
            if received_data.startswith(b'\xA5\xA5'):
                # ヘッダーを除いたデータ部分をデコードして表示
                actual_data = received_data[2:].decode('utf-8', errors='ignore').strip()
                print("Received from STM32:", actual_data)


except serial.SerialException as e:
    print("Serial error:", e)

finally:
    if ser.is_open:
        ser.close()
        print("Connection closed")
```