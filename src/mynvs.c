#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include <nvs.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "mynvs.h"

int32_t readNvsValue(char *option)
{
	ESP_ERROR_CHECK(nvs_flash_init());

	int32_t value = 0;
	nvs_handle standardPartitionHandler;

	esp_err_t res_nvs = nvs_open("armazenamento", NVS_READONLY, &standardPartitionHandler);

	if(res_nvs == ESP_ERR_NVS_NOT_FOUND)
	{
		ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
	}
	else
	{
	    esp_err_t res = nvs_get_i32(standardPartitionHandler, option, &value);

		switch (res)
		{
		case ESP_OK:
			printf("value armazenado: %d\n", value);
			break;
		case ESP_ERR_NOT_FOUND:
			ESP_LOGE("NVS", "value não encontrado");
			return -1;
		default:
			ESP_LOGE("NVS", "Erro ao acessar o NVS (%s)", esp_err_to_name(res));
			return -1;
			break;
		}

		nvs_close(standardPartitionHandler);
	}
	return value;
}

void writeNvsValue(int32_t value,char *option)
{
	ESP_ERROR_CHECK(nvs_flash_init());
	nvs_handle standardPartitionHandler;

	esp_err_t res_nvs = nvs_open("armazenamento", NVS_READWRITE, &standardPartitionHandler);

	if(res_nvs == ESP_ERR_NVS_NOT_FOUND)
	{
		ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
	}

    esp_err_t res = nvs_set_i32(standardPartitionHandler, option, value);
	
    if(res != ESP_OK)
	{
		ESP_LOGE("NVS", "Não foi possível escrever no NVS (%s)", esp_err_to_name(res));
	}
	nvs_commit(standardPartitionHandler);
	nvs_close(standardPartitionHandler);
}