
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define ESP_INTR_FLAG_DEFAULT 0
#define CONFIG_LED_PIN 2
#define CONFIG_BUTTON_PIN 0

TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;
QueueHandle_t queue1;


TaskHandle_t ISR = NULL;

// interrupt service routine, called when the button is pressed
void IRAM_ATTR button_isr_handler(void* arg) {
  char txbuff[50];
  sprintf(txbuff,"hello from ISR!");
    xQueueSendFromISR(queue1, &txbuff, NULL);

}


 
 void task1(void *arg)
{

char txbuff[50];

queue1= xQueueCreate(1, sizeof(txbuff));

 if( queue1 == 0 )
 {
    printf("failed to create queue1= %p \n",queue1); // Failed to create the queue.

 }


sprintf(txbuff,"hello world! 1");
if(xQueueSend(queue1, (void*)txbuff , (TickType_t)0 ) !=1)
	{ printf("could not sended this message = %s \n",txbuff);  }


sprintf(txbuff,"hello world! 2");
if(xQueueSend(queue1, (void*)txbuff , (TickType_t)0 ) !=1)
	{ printf("could not sended this message = %s \n",txbuff);  }

sprintf(txbuff,"hello world! 3");
if(xQueueOverwrite(queue1, (void*)txbuff ) !=1)
	{ printf("could not sended this message = %s \n",txbuff);  }



 while(1){  
  vTaskDelay(pdMS_TO_TICKS(10)); 
   printf("data waiting to be read : %d  available spaces: %d \n",uxQueueMessagesWaiting(queue1),uxQueueSpacesAvailable(queue1));

   vTaskDelay(pdMS_TO_TICKS(1000)); }
}

 void task2(void *arg)
{
char rxbuff[50];
while(1)

  { 
if(xQueueReceive(queue1, &(rxbuff) , (TickType_t)5 ))
{
printf("got a data from queue!  ===  %s \n",rxbuff); }




   vTaskDelay(pdMS_TO_TICKS(1000));
   }
}

void app_main()
{
  gpio_pad_select_gpio(CONFIG_BUTTON_PIN);
  gpio_pad_select_gpio(CONFIG_LED_PIN);
  
  // set the correct direction
  gpio_set_direction(CONFIG_BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(CONFIG_LED_PIN, GPIO_MODE_OUTPUT);
  
  // enable interrupt on falling (1->0) edge for button pin
  gpio_set_intr_type(CONFIG_BUTTON_PIN, GPIO_INTR_NEGEDGE);

  
  //Install the driver’s GPIO ISR handler service, which allows per-pin GPIO interrupt handlers.
  // install ISR service with default configuration
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  
  // attach the interrupt service routine
  gpio_isr_handler_add(CONFIG_BUTTON_PIN, button_isr_handler, NULL);





    xTaskCreate(task1, "task1", 4096, NULL, 10, &myTask1Handle);
   xTaskCreatePinnedToCore(task2, "task2", 4096, NULL, 10, &myTask2Handle,1);
}
