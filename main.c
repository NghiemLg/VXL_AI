#include "main.h"
#include "string.h"
#include "weght.h"
#include <math.h>
ETH_TxPacketConfig TxConfig;
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

ETH_HandleTypeDef heth;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ETH_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_SPI1_Init(void);
static void MX_RTC_Init(void);
void z_score_normalization(double input[], double normalized[], int size) {
    double mean = 0.0, std_dev = 0.0;
    for (int i = 0; i < size; i++) {
        mean += input[i];
    }
    mean /= size;
    for (int i = 0; i < size; i++) {
        std_dev += (input[i] - mean) * (input[i] - mean);
    }
    std_dev = sqrt(std_dev / size);
    for (int i = 0; i < size; i++) {
        normalized[i] = (input[i] - mean) / std_dev;
    }
}
void relu(double input[], double output[], int size) {
    for (int i = 0; i < size; i++) {
        output[i] = fmax(0.0, input[i]);
    }
}
void softmax(double input[], double output[], int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        output[i] = exp((int64_t)input[i]);
        sum += output[i];
    }
    for (int i = 0; i < size; i++) {
        output[i] /= sum;
    }
}
void matvec_mult(double matrix[][768], double* vector, double* result, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        result[i] = 0.0;
        for (int j = 0; j < cols; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
}
void add_bias(double vector[], double bias[], int size) {
    for (int i = 0; i < size; i++) {
        vector[i] += bias[i];
    }
}
void softma(double input[], double output[], int size) {
    double max_val = input[0];
    double sum = 0.0;

    // Tìm giá trị lớn nhất trong input
    for (int i = 1; i < size; i++) {
        if (input[i] > max_val) {
            max_val = input[i];
        }
    }

    // Tính exp(input[i] - max_val) và tổng
    for (int i = 0; i < size; i++) {
        output[i] = exp(input[i] - max_val);
        sum += output[i];
    }

    // Chuẩn hóa để tổng các giá trị bằng 1
    for (int i = 0; i < size; i++) {
        output[i] /= sum;
    }
}
void adxl_write (uint8_t Reg, uint8_t data){
	uint8_t writeBuf[2];
	writeBuf[0] = Reg|0x40;
	writeBuf[1] = data;
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_SPI_Transmit (&hspi1, writeBuf, 2,100);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}
void adxl_read (uint8_t Reg, uint8_t *Buffer, size_t len){
	Reg |= 0x80;
	Reg |= 0x40;
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &Reg,1,100);
	HAL_SPI_Receive(&hspi1, Buffer, len,100);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}
void l3g4_write (uint8_t Reg, uint8_t data){
	uint8_t writeBuf[2];
	writeBuf[0] = Reg|0x40;
	writeBuf[1] = data;
	HAL_GPIO_WritePin (GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_SPI_Transmit (&hspi1, writeBuf, 2,100);
	HAL_GPIO_WritePin (GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
}
void l3g4_read (uint8_t Reg, uint8_t *Buffer, size_t len){
	Reg |= 0x80;
	Reg |= 0x40;
	HAL_GPIO_WritePin (GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &Reg,1,100);
	HAL_SPI_Receive(&hspi1, Buffer, len,100);
	HAL_GPIO_WritePin (GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
}
void adxl_init (void){
	adxl_write (0x31, 0b1);// chỉnh độ nhạy trên cảm biến là +-2g(19.6 m2/s)
	adxl_write (0x2c, 0b1001);// chỉnh tần số đo là 50 hz
	adxl_write (0x38, 0b10101000);// enable fifo, chọn ngưỡng data là 16
	adxl_write (0x2e, 0b10);//enable ngắt watermark(gửi tín hiệu khi đủ 16 dữ li)
	adxl_write (0x2f, 0b00000000);//chỉnh cho tín hiệu ngắt được thực hiện trên chân int1
	adxl_write (0x2d, 0x08);//bật chế độ do cho cảm biến
}
void l3g4_init (void){
	uint8_t chipID=1;
	l3g4_write(0x23, 0x20);// chỉnh độ nhạy là 500 dps, tần số đo là 50 (tần số nhỏ nhát)
	l3g4_write(0x24, 0b1000000);// bật chế độ sử dụng khối fifo
	l3g4_write(0x2E, 0b00101000);//bật chế độ fifo, cho ngưỡng là 16
	l3g4_write(0x2E, 0b00101000);//bật chế độ fifo, cho ngưỡng là 16
	l3g4_write(0x22, 0b100);//enable ngắt watermark(gửi tín hiệu khi data sẵn sàng) cho chân int2
	l3g4_write(0x20, 0b1111);// bật chế độ đo cho cảm biến
}
void Reset_all(){
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
}
void standby_sensor(){
	l3g4_write(0x20, 0b0);
	adxl_write (0x2d, 0x0);
}
void wakeup_sensor(){
	l3g4_write(0x20, 0b1111);
	adxl_write (0x2d, 0x08);
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_SPI1_Init();
  MX_RTC_Init();
  adxl_init();
  l3g4_init ();

  int8_t RxData_ad[6];
  int8_t RxData_l3[6];
  double hidden1[HIDDEN1_SIZE];
  double hidden2[HIDDEN2_SIZE];
  double output[6]={0,0,0,0,0,0};
  double input[INPUT_SIZE]={};
  double normalized_input[INPUT_SIZE];
  while (1)
  {
	  for(int i=0;i<128;i++){
	      if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6) == GPIO_PIN_SET){
		  		  adxl_read (0x32, RxData_ad, 6);
		  		  input[i] = (int)(RxData_ad[1]<<8)|RxData_ad[0];
		  		  input[i+128]= (int)((RxData_ad[3]<<8)|RxData_ad[2]);
		  		  input[i+256]= (int)((RxData_ad[5]<<8)|RxData_ad[4]);
		  }
		  if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_SET){
		  		  l3g4_read (0x28, RxData_l3, 6);
		  		  input[i+384]= (int)((RxData_l3[1]<<8)|RxData_l3[0]);
		  		  input[i+512] = (int)((RxData_l3[3]<<8)|RxData_l3[2]);
		  		  input[i+640]= (int)((RxData_l3[5]<<8)|RxData_l3[4]);
		  }
	  }
	  z_score_normalization(input, normalized_input, INPUT_SIZE);
	  matvec_mult(w1, normalized_input, hidden1, HIDDEN1_SIZE, INPUT_SIZE);
	  add_bias(hidden1, bias1, HIDDEN1_SIZE);
	  relu(hidden1, hidden1, HIDDEN1_SIZE);
	  matvec_mult(w2, hidden1, hidden2, HIDDEN2_SIZE, HIDDEN1_SIZE);
	  add_bias(hidden2, bias2, HIDDEN2_SIZE);
	  relu(hidden2, hidden2, HIDDEN2_SIZE);
	  for (int i = 0; i < 6; i++) {
	          for (int j = 0; j < 64; j++) {
	              output[i] += w3[i][j] * hidden2[j];
	          }
	  }
	  add_bias(output, bias3, OUTPUT_SIZE);
	  softma(output, output, OUTPUT_SIZE);
	  double max = output[0];
	  uint8_t max_index=0;
	  for (int i = 0; i < 6; i++) {
	          if (max <  output[i]){
	              max =  output[i];
	              max_index=i;
	          }
	  }
	  Reset_all();
	  switch (max_index) {
	  case 0:
		  HAL_GPIO_WritePin (GPIOB, LD1_Pin, GPIO_PIN_SET);
	 	  break;
	  case 1:
		  HAL_GPIO_WritePin (GPIOB, LD2_Pin, GPIO_PIN_SET);
	      break;
	  case 2:
		  HAL_GPIO_WritePin (GPIOB, LD3_Pin, GPIO_PIN_SET);
	      break;
	  case 3:
		  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	      break;
	  case 4:
		  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	      break;
	  case 5:
		  HAL_GPIO_WritePin (GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	      break;
	  }
	  standby_sensor();
	  HAL_SuspendTick();
	  HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0x801, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
	  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
	  HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	  HAL_ResumeTick();
	  wakeup_sensor();
  }
}
