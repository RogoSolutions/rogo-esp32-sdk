#include "Arduino.h"
#include "IRremoteESP8266.h"
#include "IRsend.h"
#include "assert.h"
#include "IRrecv.h"
#include "IRac.h"
#include "IRtext.h"
#include "IRutils.h"

#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_reg.h"

#include "ir_common.h"
#include "ir_Daikin.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "board.h"
#include "wile_define.h"
#ifdef __cplusplus
}
#endif

#define IR_LED IR_SEND
IRac ac(IR_LED);
IRsend irsend(IR_LED); // 10

#define RMT_DEFAULT_CONFIG_TX(gpio, channel_id)      \
{                                                    \
        .rmt_mode = RMT_MODE_TX,                     \
        .channel = channel_id,                       \
        .gpio_num = gpio,                            \
        .clk_div = 80,                               \
        .mem_block_num = 1,                          \
        .flags = 0,                                  \
        .tx_config = {                               \
            .carrier_freq_hz = 38000,                \
            .carrier_level = RMT_CARRIER_LEVEL_HIGH, \
            .idle_level = RMT_IDLE_LEVEL_LOW,        \
            .carrier_duty_percent = 33,              \
            .carrier_en = false,                     \
            .loop_en = false,                        \
            .idle_output_en = true,                  \
    }                                                \
}

void ir_send_raw(uint16_t *rawIR, uint16_t irRawLength){
    // irsend.sendRaw(rawIR, irRawLength, 38000);
    vTaskDelay(10);
    rmt_config_t tx_cfg = RMT_DEFAULT_CONFIG_TX((gpio_num_t)IR_LED, (rmt_channel_t)0);
    tx_cfg.tx_config.carrier_en = true;
    tx_cfg.tx_config.carrier_duty_percent = 50;
    rmt_config(&tx_cfg);
    rmt_driver_install(tx_cfg.channel, 0, 0);
    rmt_channel_t channel = tx_cfg.channel;

    int item_num = irRawLength/2;
    rmt_item32_t item[item_num];
    // ESP_LOGI("IR", "data length: %d", irRawLength);

    uint16_t itemIndex = 0;
    for (uint16_t i=0; i<item_num; i++){
        item[i].level0 = 1;
        item[i].duration0 = rawIR[itemIndex];
        item[i].level1 = 0;
        item[i].duration1 = rawIR[itemIndex+1];
        itemIndex += 2;
        // printf("%d %d ", item[i].duration0, item[i].duration1);
    }
    //To send data according to the waveform items.
    rmt_write_items(channel, item, sizeof(item) / sizeof(item[0]), true);
    vTaskDelay(10);
    //Wait until sending is done.
    rmt_wait_tx_done(channel,1);
    vTaskDelay(10);
    rmt_driver_uninstall(channel);
}



// const uint16_t kRecvPin = 3;
// const uint16_t kRecvPin = 19;
const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
// #if DECODE_AC
const uint8_t kTimeout = 50;
// #else
// const uint8_t kTimeout = 15;
// #endif
const uint16_t kMinUnknownSize = 12;
const uint8_t kTolerancePercentage = kTolerance;
#define LEGACY_TIMING_INFO false
IRrecv irrecv(IR_RECV, kCaptureBufferSize, kTimeout, true);

void ir_begin(){
    irsend.begin();
    // ac.begin();

    // ac.initState(&acState);
    ac.next.protocol = decode_type_t::DAIKIN;  // Set a protocol to use.
    ac.next.model = 1;  // Some A/Cs have different models. Try just the first.
    ac.next.mode = stdAc::opmode_t::kCool;  // Run in cool mode initially.
    ac.next.celsius = true;  // Use Celsius for temp units. False = Fahrenheit
    ac.next.degrees = 25;  // 25 degrees.
    ac.next.fanspeed = stdAc::fanspeed_t::kMedium;  // Start the fan at medium.
    ac.next.swingv = stdAc::swingv_t::kAuto;  // Don't swing the fan up or down.
    ac.next.swingh = stdAc::swingh_t::kAuto;  // Don't swing the fan left or right.
    ac.next.light = true;  // Turn off any LED/Lights/Display that we can.
    ac.next.beep = true;  // Turn off any beep from the A/C if we can.
    ac.next.econo = false;  // Turn off any economy modes if we can.
    ac.next.filter = true;  // Turn off any Ion/Mold/Health filters if we can.
    ac.next.turbo = false;  // Don't use any turbo/powerful/etc modes.
    ac.next.quiet = false;  // Don't use any quiet/silent/etc modes.
    ac.next.sleep = -1;  // Don't set any sleep time or modes.
    ac.next.clean = false;  // Turn off any Cleaning options if we can.
    ac.next.clock = -1;  // Don't set any current time if we can avoid it.
    ac.next.power = false;  // Initially start with the unit off.
}

uint16_t ac_state_to_value(uint8_t tempAllow, uint8_t fanAllow){
    uint8_t power = (uint8_t)ac.next.power;
    uint8_t mode = CTR_AC_MODE_COOLING;
    uint8_t fan  = CTR_FAN_SPEED_NORMAL;
    uint8_t temp = (uint8_t)ac.next.degrees;

    if (ac.next.mode == stdAc::opmode_t::kAuto){
        mode = CTR_AC_MODE_AUTO;
    }
    else if (ac.next.mode == stdAc::opmode_t::kCool){
        mode = CTR_AC_MODE_COOLING;
    }
    else if (ac.next.mode == stdAc::opmode_t::kDry){
        mode = CTR_AC_MODE_DRY;
    }
    else if (ac.next.mode == stdAc::opmode_t::kFan){
        mode = CTR_AC_MODE_FAN;
    }
    else if (ac.next.mode == stdAc::opmode_t::kHeat){
        mode = CTR_AC_MODE_HEATING;
    }

    if (ac.next.fanspeed == stdAc::fanspeed_t::kAuto){
        fan = CTR_FAN_SPEED_AUTO;
    }
    else if (ac.next.fanspeed == stdAc::fanspeed_t::kHigh){
        fan = CTR_FAN_SPEED_HIGH;
    }
    else if (ac.next.fanspeed == stdAc::fanspeed_t::kLow){
        fan = CTR_FAN_SPEED_LOW;
    }
    else if (ac.next.fanspeed == stdAc::fanspeed_t::kMedium){
        fan = CTR_FAN_SPEED_NORMAL;
    }

    if (tempAllow == 1){
        temp = 15+15;
    }
    if (fanAllow == 1){
        fan = CTR_FAN_SPEED_DISABLE;
    }

    uint16_t value = 0;
    if (power){
        // value = 0xFFFF;
        uint8_t bits[16] = {1};
        uint8_t bit = 1;
        // int mask = 1 << bit;

        for (uint8_t i=0; i<3; i++){
            // mask = 1 << bit;
            // value = (value & ~mask) | (((mode >> i) & 1) << bit);
            // bit++;
            bits[bit] = (mode >> (2-i)) & 1;
            bit++;
        }
        for (uint8_t i=0; i<4; i++){
            // mask = 1 << bit;
            // value = (value & ~mask) | ((((int16_t)ac.next.degrees >> i) & 1) << bit);
            // bit++;
            bits[bit] = ((temp-15) >> (3-i)) & 1;
            bit++;
        }
        for (uint8_t i=0; i<3; i++){
            // mask = 1 << bit;
            // value = (value & ~mask) | (((fan >> i) & 1) << bit);
            // bit++;
            bits[bit] = (fan >> (2-i)) & 1;
            bit++;
        }
        while (bit < 16){
            bits[bit] = 1;
            bit++;
        }
        for (uint8_t i=0; i<16; i++){
            value += (bits[i])*pow(2, 15-i);
            // printf("%d %d\n", bits[i], value);
        }
        // printf("%d\n", value);
    }
    else {
        value = 0;
    }
    return value;
}

void ir_ac_tx_task(void *pvParameters){
    ac.sendAc();
    freeRawSave();

    // printf("AC State: Power: %d, Mode: %d, Temp: %.1f, Fan: %d\n", ac.next.power, ac.next.mode, ac.next.degrees, ac.next.fanspeed);

    vTaskDelete(NULL);
}

void ir_set_ac_state(uint16_t type, uint16_t value){
        switch (type){
            case FEATURE_ONOFF:
                if (value == 0x00){
                    ac.next.power = false;
                }
                else if (value == 0x01){
                    ac.next.power = true;
                }
                break;
            case FEATURE_MODE:
                switch (value){
                    case CTR_AC_MODE_AUTO:
                        ac.next.mode = stdAc::opmode_t::kAuto;
                        break;
                    case CTR_AC_MODE_COOLING:
                        ac.next.mode = stdAc::opmode_t::kCool;
                        break;
                    case CTR_AC_MODE_DRY:
                        ac.next.mode = stdAc::opmode_t::kDry;
                        break;
                    case CTR_AC_MODE_FAN:
                        ac.next.mode = stdAc::opmode_t::kFan;
                        break;
                    case CTR_AC_MODE_HEATING:
                        ac.next.mode = stdAc::opmode_t::kHeat;
                        break;
                }
                break;
            case FEATURE_FAN_SWING:
                switch (value){
                    case CTR_FAN_SWING_AUTO:
                        ac.next.swingv = stdAc::swingv_t::kAuto;
                        break;
                    case CTR_FAN_SWING_HIGH:
                        ac.next.swingv = stdAc::swingv_t::kHigh;
                        break;
                    case CTR_FAN_SWING_LOW:
                        ac.next.swingv = stdAc::swingv_t::kLow;
                        break;
                    case CTR_FAN_SWING_NORMAL:
                        ac.next.swingv = stdAc::swingv_t::kMiddle;
                        break;
                }
                break;
            case FEATURE_FAN_SPEED:
                switch (value){
                    case CTR_FAN_SPEED_AUTO:
                        ac.next.fanspeed = stdAc::fanspeed_t::kAuto;
                        break;
                    case CTR_FAN_SPEED_HIGH:
                        ac.next.fanspeed = stdAc::fanspeed_t::kHigh;
                        break;
                    case CTR_FAN_SPEED_LOW:
                        ac.next.fanspeed = stdAc::fanspeed_t::kLow;
                        break;
                    case CTR_FAN_SPEED_MAX:
                        ac.next.fanspeed = stdAc::fanspeed_t::kMax;
                        break;
                    case CTR_FAN_SPEED_NORMAL:
                        ac.next.fanspeed = stdAc::fanspeed_t::kMedium;
                        break;
                }
                break;
            case FEATURE_TEMP_SET:
                ac.next.degrees = value;
                break;
        }
}

void ir_send_ac(int16_t protocol, uint8_t power, uint8_t mode, uint8_t temp, uint8_t fan, uint8_t swing){
    // printf("Send AC\n");
    ESP_LOGI("IR", "Send AC: %d", protocol);
    decode_type_t proto = (decode_type_t)protocol;
    if (ac.isProtocolSupported(proto)){
        ac.next.protocol = proto;

        if (power == 0x00){
            ac.next.power = false;
        }
        else if (power == 0x01){
            ac.next.power = true;
        }

        switch (mode){
                    case CTR_AC_MODE_AUTO:
                        ac.next.mode = stdAc::opmode_t::kAuto;
                        break;
                    case CTR_AC_MODE_COOLING:
                        ac.next.mode = stdAc::opmode_t::kCool;
                        break;
                    case CTR_AC_MODE_DRY:
                        ac.next.mode = stdAc::opmode_t::kDry;
                        break;
                    case CTR_AC_MODE_FAN:
                        ac.next.mode = stdAc::opmode_t::kFan;
                        break;
                    case CTR_AC_MODE_HEATING:
                        ac.next.mode = stdAc::opmode_t::kHeat;
                        break;
        }

        switch (swing){
                    case CTR_FAN_SWING_AUTO:
                        ac.next.swingv = stdAc::swingv_t::kAuto;
                        break;
                    case CTR_FAN_SWING_HIGH:
                        ac.next.swingv = stdAc::swingv_t::kHigh;
                        break;
                    case CTR_FAN_SWING_LOW:
                        ac.next.swingv = stdAc::swingv_t::kLow;
                        break;
                    case CTR_FAN_SWING_NORMAL:
                        ac.next.swingv = stdAc::swingv_t::kMiddle;
                        break;
                    case CTR_FAN_SWING_OFF:
                        ac.next.swingv = stdAc::swingv_t::kOff;
                        break;
        }

        switch (fan){
                    case CTR_FAN_SPEED_AUTO:
                        ac.next.fanspeed = stdAc::fanspeed_t::kAuto;
                        break;
                    case CTR_FAN_SPEED_HIGH:
                        ac.next.fanspeed = stdAc::fanspeed_t::kHigh;
                        break;
                    case CTR_FAN_SPEED_LOW:
                        ac.next.fanspeed = stdAc::fanspeed_t::kLow;
                        break;
                    case CTR_FAN_SPEED_MAX:
                        ac.next.fanspeed = stdAc::fanspeed_t::kMax;
                        break;
                    case CTR_FAN_SPEED_NORMAL:
                        ac.next.fanspeed = stdAc::fanspeed_t::kMedium;
                        break;
        }

        ac.next.degrees = temp;

        // xTaskCreate(ir_ac_tx_task, "ir_ac_tx_task", 1024*4, NULL, configMAX_PRIORITIES - 1, NULL);
        board_led_operation(LED_B, LED_ON);
        if (!ac.sendAc()) ESP_LOGE("IR", "Send AC Fail");
        /* For debug ****************************************************
        uint16_t *debugRawData = NULL;
        uint16_t debugRawLen = 0;
        getRawSave(&debugRawData, &debugRawLen);
        for (uint16_t i=0; i<debugRawLen; i++) printf("%d ", debugRawData[i]);
        printf("\n");
        if (debugRawData != NULL) free(debugRawData);
        ****************************************************************/
        freeRawSave();
        ESP_LOGI("IR", "AC State: Power: %d, Mode: %d, Temp: %.1f, Fan: %d, Swing: %d\n", (int)ac.next.power, (int)ac.next.mode, ac.next.degrees, (int)ac.next.fanspeed, (int)ac.next.swingv);
        // printf("AC State: Power: %d, Mode: %d, Temp: %.1f, Fan: %d\n", ac.next.power, ac.next.mode, ac.next.degrees, ac.next.fanspeed);
        vTaskDelay(5);
        board_led_operation(LED_B, LED_OFF);
    }
    else{
        ESP_LOGE("IR", "Unsupported protocol");
        // printf("E: Unsupported protocol");
    }
}

uint8_t ir_send_value(const uint64_t data, const uint16_t nbits){
    // return irsend.send(SAMSUNG, data, nbits);
    return 0;
}

// uint8_t ir_send_daikin_152(uint8_t on, uint8_t temp){
//     if (on){
//         ac.on();
//     }
//     else{
//         ac.off();
//     }
//     ac.setFan(1);
//     ac.setMode(kDaikinCool);
//     ac.setTemp(temp);
//     ac.send();
//     return 0;
// }

void ir_recv_begin(){
    // assert(irutils::lowLevelSanityCheck() == 0);
#if DECODE_HASH
    irrecv.setUnknownThreshold(kMinUnknownSize);
#endif
    irrecv.setTolerance(kTolerancePercentage);
    irrecv.enableIRIn();
    // Serial.begin(115200);
}

uint8_t ir_decode_from_raw(uint16_t *raw, uint16_t raw_len) {
    decode_results results;
    results.rawlen = raw_len;

    results.rawbuf = new uint16_t[results.rawlen];
    for (uint16_t i=0; i<results.rawlen; i++){
        // results.rawbuf[i] =  (uint16_t)(raw[i]);
        results.rawbuf[i] =  (uint16_t)(raw[i]/2);
    }

    results.overflow = false;
    // printf("Data length: %d\n", results.rawlen);
    // printf("Data: ");
    // for (uint16_t i=0; i<results.rawlen; i++){
    //     printf("%d,", results.rawbuf[i]);
    // }
    // printf("\n");
  
    if (irrecv.decode(&results, true)){
        // printf("Decode Success\n");
        ESP_LOGI("IR", "Decode Success");
    }
    else{
        // printf("Decode Fail\n");
        ESP_LOGE("IR", "Decode Fail");
    }

    // Check if we got an IR message that was to big for our capture buffer.
    if (results.overflow)
        printf(D_WARN_BUFFERFULL "\n", kCaptureBufferSize);
    // Display the tolerance percentage if it has been change from the default.
    if (kTolerancePercentage != kTolerance)
        printf(D_STR_TOLERANCE " : %d%%\n", kTolerancePercentage);
    // Display the basic output of what we found.
    // printf("%s\n", resultToHumanReadableBasic(&results));
    printf("Code: 0x%LX, protocol: %d\n", results.value, results.decode_type);
    String str = resultToHumanReadableBasic(&results);
    char decode[str.length()];
    str.toCharArray(decode, str.length());
    printf("%s\n", decode);

    // Display any extra A/C info if we have it.
    // String description = IRAcUtils::resultAcToString(&results);
    // if (description.length()){
    //     // print(D_STR_MESGDESC ": " + description);
    //     ESP_LOGI("IR", "%s\n", description);
    // }
    // yield();  // Feed the WDT as the text output can take a while to print.



    // Output the results as source code
    // print(resultToSourceCode(&results));
    // ESP_LOGI("IR", "%s\n", resultToSourceCode(&results));
    printf("\n");    // Blank line between entries
    // yield();

    // if (irsend.send(results.decode_type, results.value, results.bits)){
    //     printf("Send Success\n");
    // }
    // else{
    //     printf("Send Fail\n");
    // }

    return 0;
}

int8_t ir_decode(int16_t *protocol, char **code, uint16_t **rawData, uint16_t *rawLen){
    ESP_LOGW("IR", "Decode start in 5s...");
    irrecv.resume();
    decode_results results;
    uint32_t lastTime = xTaskGetTickCount();
    bool decode_success = false;
    *protocol = UNKNOWN;
    while(xTaskGetTickCount() - lastTime <= 5000){
        decode_success = irrecv.decode(&results, false);
        // if (irrecv.decode(&results, false)){
        if (decode_success){
            // printf("Decode Success\n");
            ESP_LOGI("IR", "Decode Success");

            // Check if we got an IR message that was to big for our capture buffer.
            if (results.overflow)
                printf(D_WARN_BUFFERFULL "\n", kCaptureBufferSize);
            // Display the tolerance percentage if it has been change from the default.
            if (kTolerancePercentage != kTolerance)
                printf(D_STR_TOLERANCE " : %d%%\n", kTolerancePercentage);
            // Display the basic output of what we found.
            // printf("%s\n", resultToHumanReadableBasic(&results));
            *protocol = results.decode_type;
            // printf("Code: 0x%X, protocol: %d\n", results.value, results.decode_type);
            
            // if (*rawData != NULL){
            //     free(*rawData);
            //     *rawData = NULL;
            // }
            // *rawLen = getCorrectedRawLength(&results);
            // *rawData = resultToRawArray(&results);
            // for (uint16_t i=0; i<*rawLen; i++){
            //     printf("%d ", *(*rawData+i));
            // }
            // printf("\n");

            // for (uint16_t i=0; i<results.rawlen; i++){
            //     printf("%d, ", 2*results.rawbuf[i]);
            // }
            // printf("\n");

            if (*code != NULL){
                free(*code);
                *code = NULL;
            }
            String codeStr = resultToHexidecimal(&results);
            *code = (char *)malloc(codeStr.length()+1);
            codeStr.toCharArray(*code, codeStr.length()+1);
            // printf("\nCode: %s\n", *code);
            ESP_LOGI("IR", "Code: %s", *code);

            String str = resultToHumanReadableBasic(&results);
            char decode[str.length()+1];
            str.toCharArray(decode, str.length()+1);
            // printf("%s\n", decode);
            ESP_LOGW("IR", "%s", decode);

            // Display any extra A/C info if we have it.
            String description = IRAcUtils::resultAcToString(&results);
            if (description.length()){
                char descStr[description.length()+1];
                description.toCharArray(descStr, description.length()+1);
                // print(D_STR_MESGDESC ": " + description);
                ESP_LOGI("IR", "%s\n", descStr);
            }
            break;
        }
        vTaskDelay(1);
        // else{
        //     printf("Decode Fail\n");
        // }
    }
    irrecv.pause();
    ESP_LOGI("IR", "Decode stop\n");
    if (decode_success && *protocol == UNKNOWN){
        // decode_success = false;
        // printf("Unknown raw save\n");
        // ESP_LOGW("IR", "Unknown raw save");
        *rawLen = getCorrectedRawLength(&results);
        if (*rawData != NULL){
            free(*rawData);
            *rawData = NULL;
        }
        *rawData = resultToRawArray(&results);
        // for (uint16_t i=0; i<*rawLen; i++){
        //     printf("%d ", *(*rawData+i));
        // }
        // printf("\n");
    }
    else if (decode_success){
        freeRawSave();
        // irsend.begin();
        if (hasACState(results.decode_type)){  // Is it larger than 64 bits?
            if (results.bits % 8 == 0){
                irsend.send(results.decode_type, results.state, results.bits/8);
            }
            else{
                irsend.send(results.decode_type, results.state, results.bits/8+1);
            }
        }
        else{
            irsend.send(results.decode_type, results.value, results.bits, kNoRepeat);
        }
        if (*rawData != NULL){
            free(*rawData);
            *rawData = NULL;
        }
        getRawSave(rawData, rawLen);

        // *rawLen = getCorrectedRawLength(&results);
        // if (*rawData != NULL){
        //     free(*rawData);
        //     *rawData = NULL;
        // }
        // *rawData = resultToRawArray(&results);
        // *(*rawData+*rawLen) = 25000;
        // ir_send_raw(*rawData, *rawLen+1);
        // for (uint16_t i=0; i<*rawLen+1; i++){
        //     printf("%d,", *(*rawData+i));
        // }
        // printf("\n");
    }
    // if (!ac.isProtocolSupported((decode_type_t)(*protocol))) *protocol = -1;
    return decode_success;
    // vTaskDelete(NULL);
}

void ir_decode_to_state_task(void *pvParameters){
    // decode_results *ir_decode = (decode_results *)pvParameters;
    decode_results ir_decode;
    memcpy(&ir_decode, pvParameters, sizeof(decode_results));
    // ESP_LOGW("IR 2way", "results.decode_type: %d", ir_decode->decode_type);
    uint16_t eid = nwk_addr_to_eid(ir_decode.decode_type);
    ESP_LOGW("IR 2way", "EID: %d", eid);

    stdAc::state_t recvAcState;
    if (eid != 0 && IRAcUtils::decodeToState(&ir_decode, &recvAcState)){
        ESP_LOGW("IR 2way", "power: %d", (int)recvAcState.power);
        ESP_LOGW("IR 2way", "mode: %d", (int)recvAcState.mode);
        ESP_LOGW("IR 2way", "degrees: %.1f", recvAcState.degrees);
        ESP_LOGW("IR 2way", "fanspeed: %d", (int)recvAcState.fanspeed);
        ESP_LOGW("IR 2way", "swing: %d", (int)recvAcState.swingv);
        uint8_t ir_ac_state[5] = {0};
        ir_ac_state[0] = recvAcState.power;
        switch ((int8_t)recvAcState.mode){
            case (int8_t)stdAc::opmode_t::kAuto:
            case (int8_t)stdAc::opmode_t::kCool:
            case (int8_t)stdAc::opmode_t::kFan:
                ir_ac_state[1] = (int8_t)recvAcState.mode;
                break;
            case (int8_t)stdAc::opmode_t::kDry:
                ir_ac_state[1] = CTR_AC_MODE_DRY;
                break;
            case (int8_t)stdAc::opmode_t::kHeat:
                ir_ac_state[1] = CTR_AC_MODE_HEATING;
                break;
        }
        ir_ac_state[2] = (int8_t)recvAcState.degrees;
        switch ((int8_t)recvAcState.fanspeed){
            case (int8_t)stdAc::fanspeed_t::kAuto:
            case (int8_t)stdAc::fanspeed_t::kMin:
                ir_ac_state[3] = (int8_t)recvAcState.fanspeed;
                break;
            default:
                ir_ac_state[3] = (int8_t)recvAcState.fanspeed-1;
                break;
        }
        switch ((int8_t)recvAcState.swingv){
            case (int8_t)stdAc::swingv_t::kOff:
            case (int8_t)stdAc::swingv_t::kAuto:
                ir_ac_state[4] = (int8_t)recvAcState.swingv;
                break;
            default:
                ir_ac_state[4] = 5-(int8_t)recvAcState.swingv;
                break;
        }
        set_device_state(eid, FEATURE_AC_SHORT, ir_ac_state, true);
    }
    else{
        ESP_LOGW("IR 2way", "Isn't AC");
    }
    vTaskDelete(NULL);
}

void ir_decode_idle_task(void *pvParameters){
    ESP_LOGW("IR", "Decode start...");
    decode_results results;
    bool decode_success = false;
    int16_t protocol = UNKNOWN;
    while(1){
        decode_success = irrecv.decode(&results, false);
        // if (irrecv.decode(&results, false)){
        if (decode_success){
            // printf("Decode Success\n");
            // ESP_LOGI("IR", "Decode Success");

            // Check if we got an IR message that was to big for our capture buffer.
            if (results.overflow)
                printf(D_WARN_BUFFERFULL "\n", kCaptureBufferSize);
            // Display the tolerance percentage if it has been change from the default.
            if (kTolerancePercentage != kTolerance)
                printf(D_STR_TOLERANCE " : %d%%\n", kTolerancePercentage);
            // Display the basic output of what we found.
            // printf("%s\n", resultToHumanReadableBasic(&results));
            protocol = results.decode_type;
            // printf("Code: 0x%X, protocol: %d\n", results.value, results.decode_type);
            
            // String codeStr = resultToHexidecimal(&results);
            // *code = (char *)malloc(codeStr.length()+1);
            // codeStr.toCharArray(*code, codeStr.length()+1);
            // // printf("\nCode: %s\n", *code);
            // ESP_LOGI("IR", "Code: %s", codeStr);
            if (protocol != UNKNOWN){
                if (protocol == MITSUBISHI_HEAVY_160) ESP_LOGW("IR", "MITSUBISHI_HEAVY_160");
                String str = resultToHumanReadableBasic(&results);
                char decode[str.length()+1];
                str.toCharArray(decode, str.length()+1);
                // printf("%s\n", decode);
                ESP_LOGW("IR", "%s", decode);

                // Display any extra A/C info if we have it.
                String description = IRAcUtils::resultAcToString(&results);
                if (description.length()){
                    char descStr[description.length()+1];
                    description.toCharArray(descStr, description.length()+1);
                    // print(D_STR_MESGDESC ": " + description);
                    ESP_LOGI("IR", "%s\n", descStr);
                }

                xTaskCreate(&ir_decode_to_state_task, "ir_decode_to_state_task", 3*1024, &results, 4, NULL);
                stdAc::state_t recvAcState;
                if (IRAcUtils::decodeToState(&results, &recvAcState)){
                    ESP_LOGW("IR 2way", "Power: %d", recvAcState.power);
                }
            }
            /* For debug ****************************************************
            if (protocol == UNKNOWN){
                uint16_t rawLen = getCorrectedRawLength(&results);
                uint16_t *rawData = resultToRawArray(&results);
                printf("\"");
                for (uint16_t i=0; i<rawLen; i++){
                    printf("%d", *(rawData+i));
                    if (i < rawLen-1) printf(", ");
                }
                printf("\"\n");
                if (rawData != NULL) free(rawData);
            }
            ****************************************************************/
        }
        // vTaskDelay(1);
        // else{
        //     printf("Decode Fail\n");
        // }
    }
    vTaskDelete(NULL);
}

int8_t ir_send_prtc(int16_t protocol, uint64_t code){
    // freeRawSave();
    // decode_type_t irProto = (decode_type_t)protocol;
    // uint8_t ret = irsend.send(irProto, code, irsend.defaultBits(irProto), irsend.minRepeats(irProto));
    // uint16_t rawLen = 0;
    // uint16_t *rawData = NULL;
    // getRawSave(&rawData, &rawLen);

    // ESP_LOG_BUFFER_HEX("IR PRTC", rawData, rawLen);

    // board_led_operation(LED_B, LED_ON);
    // ir_send_raw(rawData, rawLen);

    // vTaskDelay(5);
    // board_led_operation(LED_B, LED_OFF);

    // if (rawData != NULL){
    //     free(rawData);
    //     rawData = NULL;
    // }
    // return ESP_OK;

    decode_type_t irProto = (decode_type_t)protocol;
    board_led_operation(LED_B, LED_ON);
    uint8_t ret = irsend.send(irProto, code, irsend.defaultBits(irProto), irsend.minRepeats(irProto));
    vTaskDelay(5);
    board_led_operation(LED_B, LED_OFF);
    freeRawSave();
    return ret;
}

void ir_proto_meaning(int16_t protocol, int16_t *type, int16_t *manu){
    *type = DEVICE_TYPE_OTHER;

    switch ((decode_type_t)protocol){
    case UNKNOWN:
        *manu = MANU_UNKNOWN;
        break;
    case DAIKIN128:
    case DAIKIN152:
    case DAIKIN160:
    case DAIKIN176:
    case DAIKIN200:
    case DAIKIN216:
    case DAIKIN2:
    case DAIKIN312:
    case DAIKIN64:
    case DAIKIN:
        *manu = MANU_DAIKIN;
        break;
    case PANASONIC:
        *type = DEVICE_TYPE_TV;
        *manu = MANU_PANASONIC;
        break;
    case PANASONIC_AC:
    case PANASONIC_AC32:
    case PANASONIC_AC128:
        *manu = MANU_PANASONIC;
        break;
    case MITSUBISHI:
    case MITSUBISHI2:
        *type = DEVICE_TYPE_TV;
        *manu = MANU_MITSUBISHI;
        break;
    case MITSUBISHI_AC:
    case MITSUBISHI112:
    case MITSUBISHI136:
        *manu = MANU_MITSUBISHI;
        break;
    case MITSUBISHI_HEAVY_88:
    case MITSUBISHI_HEAVY_152:
    case MITSUBISHI_HEAVY_160:
        *manu = MANU_MITSUBISHI_INDUSTRY;
        break;
    case TOSHIBA_AC:
        *manu = MANU_TOSHIBA;
        break;
    case MIDEA:
        *manu = MANU_MIDEA;
        break;
    case MIDEA24:
        *type = DEVICE_TYPE_FAN;
        *manu = MANU_MIDEA;
        break;
    case GREE:
        *manu = MANU_GREE;
        break;
    case HITACHI_AC:
    case HITACHI_AC1:
    case HITACHI_AC2:
    case HITACHI_AC3:
    case HITACHI_AC264:
    case HITACHI_AC296:
    case HITACHI_AC344:
    case HITACHI_AC424:
        *manu = MANU_HITACHI;
        break;
    case HAIER_AC:
    case HAIER_AC160:
    case HAIER_AC176:
    case HAIER_AC_YRW02:
        *manu = MANU_HAIER;
        break;
    // case WHIRLPOOL_AC:
        // *manu = MANU_HINSENSE;
        // break;
    case SAMSUNG:
    case SAMSUNG36:
        *type = DEVICE_TYPE_TV;
        *manu = MANU_SAMSUNG;
        break;
    case SAMSUNG_AC:
        *manu = MANU_SAMSUNG;
        break;
    case LG:
    case LG2:
        *manu = MANU_LG;
        break;
    case SHARP:
    case SHARP_AC:
        *manu = MANU_SHARP;
        break;
    case ELECTRA_AC:
        *manu = MANU_CASPER;
        break;
    case NEC:
        *manu = MANU_NEC;
        break;
    case SYMPHONY:
        *type = DEVICE_TYPE_FAN;
        *manu = MANU_FAN_GENERIC;
        break;
    default:
        *manu = MANU_OTHER;
        break;
    }

    if (ac.isProtocolSupported((decode_type_t)protocol)) *type = DEVICE_TYPE_AC;
}

struct irBlock{
    uint16_t start;
    uint16_t end;
    uint8_t *data;
};

void ir_full_raw_block_handler(uint8_t *cmd, uint16_t irDataRawBlock){
    uint16_t irRawLength = ((cmd[irDataRawBlock+1] << 8) | cmd[irDataRawBlock+2])/2;
    uint16_t *irRawTest = (uint16_t *)malloc(2*irRawLength+1);
    uint16_t dataIndex = 3;
    for (uint16_t i=0; i<irRawLength; i++){
        irRawTest[i] = (cmd[irDataRawBlock+dataIndex] << 8) | cmd[irDataRawBlock+dataIndex+1];
        dataIndex += 2;
    }
    ESP_LOGI("IR", "RAW FULL: %d", irRawLength);
    for (uint16_t i=0; i<irRawLength; i++){
        printf("%d,", irRawTest[i]);
    }
    printf("\n");
    ir_send_raw(irRawTest, irRawLength);
    free(irRawTest);
}

void ir_raw_block_handler(uint8_t *cmd, uint16_t irHeaderBlock, uint16_t irDataBlock){
    uint16_t irNumberSet = 0;
    uint8_t bitSize = 0;
    if (cmd[irHeaderBlock+3] == 0x0A){ // Compress Type
        irNumberSet = (cmd[irHeaderBlock+4] << 8) | cmd[irHeaderBlock+5];
    }
    for (uint8_t i=0; i<16; i++){
        if (irNumberSet > pow(2,i) && irNumberSet <= pow(2,i+1)){
            bitSize = i+1;
        }
    }
    ESP_LOGI("IR code", "IR number set: %d, bitSize: %d", irNumberSet, bitSize);
    uint16_t irSet[irNumberSet];
    uint16_t index = irHeaderBlock+6;
    // printf("[");
    for (uint16_t i=0; i<irNumberSet; i++){
        irSet[i] = (cmd[index] << 8) | cmd[index+1];
        // printf("%d ", irSet[i]);
        index += 2;
    }
    // printf("]\r\n");
    ESP_LOG_BUFFER_HEX("IR Number set", irSet, irNumberSet);
    uint8_t irNumberBlock = cmd[index];
    index++;
    struct irBlock irBlockArr[irNumberBlock];
    ESP_LOGI("IR code", "IR block: %d", irNumberBlock);
    uint16_t irBinaryLength = 0;
    if (irNumberBlock > 0){
        for (uint8_t i=0; i<irNumberBlock; i++){
            irBlockArr[i].start = (cmd[index] << 8) | cmd[index+1];
            irBlockArr[i].end = (cmd[index+2] << 8) | cmd[index+3];
            irBlockArr[i].data = &cmd[index+4];

            // printf("start: %d, end: %d\r\n", irBlockArr[i].start, irBlockArr[i].end);
            // printf("[");
            // for (uint8_t j=0; j<(irBlockArr[i].end - irBlockArr[i].start + 1); j++){
            //     printf("0x%02X ", irBlockArr[i].data[j]);
            // }
            // printf("]\r\n");

            irBinaryLength += irBlockArr[i].end - irBlockArr[i].start + 1;
            index += irBlockArr[i].end - irBlockArr[i].start + 1 + 4;
        }
    }

    uint16_t irRawLength = (cmd[irDataBlock+3] << 8) | cmd[irDataBlock+4];
    irBinaryLength += (cmd[irDataBlock+5] << 8) | cmd[irDataBlock+6];// Length in byte
    uint16_t dataIndex = irDataBlock+7;

    uint8_t bitIndex = 0;
    // uint8_t dataIndexBias = 0;

    uint16_t rawIR[irRawLength];
    // uint16_t rawIR[irRawLength+1]; //+1 for decode
    uint16_t rawIndex = 0;
    uint8_t blockIndex = 0;

    uint8_t binaryIndexArr[irBinaryLength];
    uint16_t binaryIndex = 0;
    while (binaryIndex < irBinaryLength){
        if (irNumberBlock != 0 && binaryIndex == irBlockArr[blockIndex].start){
            for (uint8_t i=0; i < (irBlockArr[blockIndex].end - irBlockArr[blockIndex].start + 1); i++){
                binaryIndexArr[binaryIndex] = irBlockArr[blockIndex].data[i];
                binaryIndex++;
            }
            blockIndex++;
        }
        binaryIndexArr[binaryIndex] = cmd[dataIndex];
        dataIndex++;
        binaryIndex++;
    }

    binaryIndex = 0;
    uint16_t binaryIndexBias = 0;

    // for (uint8_t index=0; index<3; index++){
    while(rawIndex < irRawLength){
        uint16_t irNumberSetIndex = 0;
        bool bitArray[bitSize];

        for (int8_t i=0; i<bitSize; i++){
            if (bitIndex < 8){
                bitArray[i] = (binaryIndexArr[binaryIndex+binaryIndexBias] >> (7-bitIndex)) & 1;
                bitIndex++;
            }
            else{
                bitIndex=0;
                binaryIndexBias++;
                bitArray[i] = (binaryIndexArr[binaryIndex+binaryIndexBias] >> (7-bitIndex)) & 1;
                bitIndex++;
            }
            // printf("%d", bitArray[i]);
            irNumberSetIndex += bitArray[i]*pow(2,bitSize-i-1);
        }
        // printf("%d ", irSet[irNumberSetIndex]);
        // printf("\r\n");
        rawIR[rawIndex] = irSet[irNumberSetIndex];
        rawIndex++;
    }
    /* For debug *****************************/
    for (uint16_t i=0; i<irRawLength; i++){
        printf("%d ", rawIR[i]);
    }
    printf("\r\n");
    /*****************************************/
    ESP_LOG_BUFFER_HEX("IR RAW", rawIR, irRawLength);

    // printf("Send IR Raw\r\n");
    ESP_LOGI("IR", "Send IR Raw");
    board_led_operation(LED_B, LED_ON);
    ir_send_raw(rawIR, irRawLength);

    // for (uint16_t i=irRawLength; i>0; i--){
    //     rawIR[i] = rawIR[i-1];
    // }
    // rawIR[0] = 2;
    // ir_decode_from_raw(rawIR, irRawLength);

    // xTaskCreate(ir_decode_task, "ir_rx_task", 2048, NULL, tskIDLE_PRIORITY, NULL);
                    
    vTaskDelay(10);
    board_led_operation(LED_B, LED_OFF);
}

int16_t irProtocol = -1;
uint16_t *irRawData = NULL;
char *irCode = NULL;

void ir_decode_task(void *pvParameters){
    // uint8_t *cmd = (uint8_t *)pvParameters;
    struct rogoIotCmd *msgCmdInfo = (struct rogoIotCmd *)pvParameters;
    uint8_t *cmd = &(msgCmdInfo->cmd[0]);
    uint16_t portTcp = cmd[5] << 8 | cmd[6];
    char *ipAddr = NULL;
    if (msgCmdInfo->ipAddr != NULL){
        ipAddr = &(msgCmdInfo->ipAddr[0]);
    }
    char *appID = (char *)malloc(cmd[10]*2+1);
    for (uint8_t i=0; i<cmd[10]; i++){
        sprintf(appID+(i*2), "%02x", cmd[11+i]);
    }
    // irCode = malloc(1);
    // irRawData = malloc(1);
    uint16_t irRawDataLen = 0;
    if (!ir_decode(&irProtocol, &irCode, &irRawData, &irRawDataLen)){
        mqtt_send_notify_status(MSG_NOTIFY_FAILURE, ERR_RG_TIMEOUT, cmd, appID, ipAddr, portTcp);
        // mqtt_send_notify_status(MSG_NOTIFY_FAILURE, ERR_RG_TIMEOUT, cmd, appID, NULL, 0);
    }
    else if (irCode == NULL || irRawData == NULL){
    // else if (irCode == NULL){
        if (irCode == NULL){
            ESP_LOGE("IR", "irCode is NULL");
        }
        if (irRawData == NULL){
            ESP_LOGE("IR", "irData is NULL");
        }
        mqtt_send_notify_status(MSG_NOTIFY_FAILURE, ERR_RG_UNDERFINE, cmd, appID, ipAddr, portTcp);
        // mqtt_send_notify_status(MSG_NOTIFY_FAILURE, ERR_RG_UNDERFINE, cmd, appID, NULL, 0);
    }
    else{
        int16_t manu;
        int16_t irDeviceType;
        ir_proto_meaning(irProtocol, &irDeviceType, &manu);
        ESP_LOGI("IR", "Manufacture: %d, Type: %d, Protocol: %d", manu, irDeviceType, irProtocol);

        struct rogoIotMsg msgResponse = { 
            .buf = NULL,
            .len = 0,
        };

        // rgmsg_create_header(&msgResponse, MSG_NOTIFY_MSG, CMD_IR, CMDs_IR_PROTOCOL_DETECTED, 0x00, 0x00);
        // rgmsg_add_full_header(&msgResponse, cmd);

        if (irDeviceType == DEVICE_TYPE_AC){
            // for (uint16_t i=0; i<irRawDataLen; i++){
            //     // printf("%02x ", (uint8_t)(irRawData[i] >> 8));
            //     // printf("%02x ", (uint8_t)irRawData[i]);
            //     printf("%d,", irRawData[i]);
            // }
            // printf("\n");
            // ESP_LOG_BUFFER_HEX("IR RAW", irRawData, irRawDataLen);

            ESP_LOGI("IR", "AC: %s", irCode);
            rgmsg_create_header(&msgResponse, MSG_NOTIFY_MSG, CMD_IR, CMDs_IR_PROTOCOL_DETECTED, (uint8_t)(irProtocol >> 8), (uint8_t)irProtocol);
            rgmsg_add_full_header(&msgResponse, cmd);

            uint8_t blockProtoInfo[10] = {0x00};
            uint8_t blockIndex = 0;

            // blockProtoInfo[blockIndex++] = 0x00;
            blockProtoInfo[++blockIndex]  = ROGO_IR_PRTC_AC; // rogoIrProtocol
            blockProtoInfo[blockIndex+=2] = 0x07; // nwkProtocolType IR
            blockProtoInfo[++blockIndex]  = (uint8_t)(irDeviceType >> 8);
            blockProtoInfo[++blockIndex]  = (uint8_t)irDeviceType;
            blockIndex += 2;
            blockProtoInfo[++blockIndex]  = (uint8_t)(manu >> 8);
            blockProtoInfo[++blockIndex]  = (uint8_t)manu;
                
            rgmsg_add_block(&msgResponse, BLOCK_PROTOCOL_INFO, sizeof(blockProtoInfo), blockProtoInfo, rootEID);

            uint8_t blockAttr[10] = {0x00};
            blockIndex = 0;

            // blockAttr[blockIndex++] = 0x00;
            blockAttr[++blockIndex]  = FEATURE_ONOFF;
            blockAttr[blockIndex+=2] = FEATURE_MODE;
            blockAttr[blockIndex+=2] = FEATURE_TEMP_SET;
            blockAttr[blockIndex+=2] = FEATURE_FAN_SPEED;
            blockAttr[blockIndex+=2] = FEATURE_FAN_SWING;

            rgmsg_add_block(&msgResponse, BLOCK_ATTRS, sizeof(blockAttr), blockAttr, rootEID);
        
            #ifdef CONFIG_IR_COLLECT_DATA_ENABLE
            uint8_t *irRawSend = (uint8_t *)calloc(irRawDataLen*2, sizeof(uint8_t));
            uint16_t irRawIdx = 0;
            // irRawSend[irRawIdx++] = irRawData[irRawDataLen-1] >> 8;
            // irRawSend[irRawIdx++] = irRawData[irRawDataLen-1];
            // irRawSend[irRawIdx++] = irRawData[0] >> 8;
            // irRawSend[irRawIdx++] = irRawData[0];
            for (uint16_t i=0; i<irRawDataLen; i++){
                irRawSend[irRawIdx++] = irRawData[i] >> 8;
                irRawSend[irRawIdx++] = irRawData[i];
            }
            rgmsg_add_block(&msgResponse, BLOCK_LARGE_DATA_RAW, 2*irRawDataLen, irRawSend, rootEID);
            free(irRawSend);
            #endif
        }
        // else if (manu == MANU_NEC || manu == MANU_FAN_GENERIC){
        else if (irProtocol != UNKNOWN && strlen(irCode) <= 18){
            if (manu == MANU_NEC) ESP_LOGI("IR", "NEC: %s", irCode);
            if (manu == MANU_FAN_GENERIC) ESP_LOGI("IR", "SYMPHONY: %s", irCode);
            rgmsg_create_header(&msgResponse, MSG_NOTIFY_MSG, CMD_IR, CMDs_IR_PROTOCOL_DETECTED, (uint8_t)(irProtocol >> 8), (uint8_t)irProtocol);
            rgmsg_add_full_header(&msgResponse, cmd);

            uint8_t blockProtoInfo[10] = {0x00};
            uint8_t blockIndex = 0;

            // blockProtoInfo[blockIndex++] = 0x00;
            blockProtoInfo[++blockIndex]  = ROGO_IR_PRTC; // rogoIrProtocol
            blockProtoInfo[blockIndex+=2] = 0x07; // nwkProtocolType IR
            blockProtoInfo[++blockIndex]  = (uint8_t)(irDeviceType >> 8);
            blockProtoInfo[++blockIndex]  = (uint8_t)irDeviceType;
            blockIndex += 2;
            blockProtoInfo[++blockIndex]  = (uint8_t)(manu >> 8);
            blockProtoInfo[++blockIndex]  = (uint8_t)manu;
                
            rgmsg_add_block(&msgResponse, BLOCK_PROTOCOL_INFO, sizeof(blockProtoInfo), blockProtoInfo, rootEID);
        
            char *index = irCode + 2;
            uint8_t codeLength = (strlen(irCode)-2)/2;
            if (strlen(irCode) % 2){
                irCode[1] = '0';
                index = irCode + 1;
                codeLength = (strlen(irCode)-1)/2;
            }
            uint8_t *blockDataRaw = (uint8_t *)malloc(codeLength);

            for (uint8_t i=0; i<codeLength; i++){
                sscanf(index, "%2hhx", &blockDataRaw[i]);
                index += 2;
            }
            rgmsg_add_block(&msgResponse, BLOCK_DATA_RAW, codeLength, blockDataRaw, rootEID);
            free(blockDataRaw);

            #ifdef CONFIG_IR_COLLECT_DATA_ENABLE
            ESP_LOG_BUFFER_HEX("IR RAW", irRawData, irRawDataLen);
            uint8_t *irRawSend = (uint8_t *)malloc(irRawDataLen*2);
            uint8_t irRawIdx = 0;
            for (uint16_t i=0; i<irRawDataLen; i++){
                irRawSend[irRawIdx++] = irRawData[i] >> 8;
                irRawSend[irRawIdx++] = irRawData[i];
            }
            rgmsg_add_block(&msgResponse, BLOCK_LARGE_DATA_RAW, 2*irRawDataLen, irRawSend, rootEID);
            free(irRawSend);
            #endif
        }
        // For raw
        else{
            rgmsg_create_header(&msgResponse, MSG_NOTIFY_MSG, CMD_IR, CMDs_IR_DATA_LEARNED, (uint8_t)(irProtocol >> 8), (uint8_t)irProtocol);
            rgmsg_add_full_header(&msgResponse, cmd);

            // for (uint16_t i=0; i<irRawDataLen; i++){
            //     printf("%d ", irRawData[i]);
            // }
            // printf("\n");

            uint8_t *irRawSend = (uint8_t *)malloc(irRawDataLen*2);
            uint16_t irRawIdx = 0;
            for (uint16_t i=0; i<irRawDataLen; i++){
                irRawSend[irRawIdx++] = irRawData[i] >> 8;
                irRawSend[irRawIdx++] = irRawData[i];
            }

            // for (uint16_t i=0; i<irRawDataLen*2; i+=2){
            //     // printf("%02X ", irRawSend[i]);
            //     printf("%d ", irRawSend[i] << 8 | irRawSend[i+1]);
            // }
            // printf("\n");

            rgmsg_add_block(&msgResponse, BLOCK_LARGE_DATA_RAW, 2*irRawDataLen, irRawSend, rootEID);
            free(irRawSend);
        }

        wile_send_to_app(appID, 2*cmd[10], (char *)(msgResponse.buf), msgResponse.len, ipAddr, portTcp);
        // wile_send_to_app(appID, 2*cmd[10], (char *)(msgResponse.buf), msgResponse.len, NULL, 0);
        if (msgResponse.buf != NULL) free(msgResponse.buf);
    }
// ir_decode_task_end:
    free(appID);
    if (irRawData != NULL){
        free(irRawData);
        irRawData = NULL;
    }
    if (irCode != NULL){
        free(irCode);
        irCode = NULL;
    }
    ESP_LOGI(TAG, "[APP] Free memory: %ld bytes", esp_get_free_heap_size());
    vTaskDelete(NULL);
}


// EXTERN API
uint8_t irSendDone = true;

extern uint8_t send_ir_raw_nvs(char *eid, char *dataName){
    uint8_t status = 1;
    nvs_handle_t ir_nvs_open_handler;
    ESP_ERROR_CHECK(nvs_open_from_partition("rogo", eid, NVS_READONLY, &ir_nvs_open_handler));
    size_t dataLen = 0;
    // nvs_get_item_size(nvs_open_handler, dataName, NVS_TYPE_BLOB, &dataLen);
    if (nvs_get_item_size(ir_nvs_open_handler, dataName, NVS_TYPE_BLOB, &dataLen) == ESP_OK){
        uint8_t *nvsData = (uint8_t *)malloc(dataLen);
        size_t headerLen = 0;
        char header[6];
        itoa(65534, header, 10);
        nvs_get_item_size(ir_nvs_open_handler, header, NVS_TYPE_BLOB, &headerLen);
        // uint16_t headerLenT = (uint16_t)headerLen;
        // uint16_t dataLenT = (uint16_t)dataLen;
        uint8_t *headerData = (uint8_t *)malloc(headerLen);
        ESP_ERROR_CHECK(nvs_get_blob(ir_nvs_open_handler, "65534", headerData, &headerLen));
        ESP_ERROR_CHECK(nvs_get_blob(ir_nvs_open_handler, dataName, nvsData, &dataLen));
        // ESP_ERROR_CHECK(nvs_commit(ir_nvs_open_handler));
        // nvs_close(ir_nvs_open_handler);
        // ESP_LOG_BUFFER_HEX("IR Data", nvsData, dataLen);
        // ESP_LOG_BUFFER_HEX("IR Header", headerData, headerLen);

        ESP_LOGI("IR", "Header Length: %d, Data Length: %d", headerLen, dataLen);

        uint8_t *irData = (uint8_t *)malloc(3+headerLen+3+dataLen+1);
        irData[0] = BLOCK_IR_CMD_HEADER;
        // irData[1] = (uint8_t)((uint16_t)headerLen >> 8);
        // irData[2] = (uint8_t)headerLen;
        memcpy(irData+3, headerData, headerLen);
        irData[3+headerLen] = BLOCK_IR_CMD_DATA;
        // // irData[3+headerLen+1] = (uint8_t)((uint16_t)dataLen >> 8);
        // // irData[3+headerLen+2] = (uint8_t)headerLen;
        memcpy(irData+3+headerLen+3, nvsData, dataLen);

        // ESP_LOG_BUFFER_HEX("IR Data final", irData, 3+headerLen+3+dataLen+1);

        ir_raw_block_handler(irData, 0, headerLen+3);
        free(irData);
        free(nvsData);
        free(headerData);
    }
    else{
        ESP_LOGE("IR", "Dataname not found");
        status = 0;
    }
    nvs_close(ir_nvs_open_handler);
    irSendDone = true;
    return status;
}

extern void send_ir_prtc_nvs(void *pvParameters){
    char *controlPara = (char *)pvParameters;
    char eid[6];
    memcpy(eid, controlPara, 6);
    char dataName[6];
    memcpy(dataName, controlPara+6, 6);

    ESP_LOGI("IR PRTC", "EID: %s, Dataname: %s", eid, dataName);

    nvs_handle_t ir_nvs_open_handler;
    ESP_ERROR_CHECK(nvs_open_from_partition("rogo", eid, NVS_READONLY, &ir_nvs_open_handler));
    uint64_t irCode = 0;
    esp_err_t err = ESP_OK;
    err = nvs_get_u64(ir_nvs_open_handler, dataName, &irCode);
    if (err != ESP_OK){
        ESP_LOGE("IR", "Dataname not found");
    }
    else{
        // int16_t irProto = 0;
        uint16_t irProto = 0;
        nvs_get_u16(ir_nvs_open_handler, eid, &irProto);
        // err = nvs_get_u16(ir_nvs_open_handler, "protocol", devProto);
        ESP_LOGI("IR", "irProto: %d, irCode: %LX", irProto, irCode);
        ir_send_prtc(irProto, irCode);
    }

    nvs_close(ir_nvs_open_handler);
    irSendDone = true;
    // return ESP_OK;
    vTaskDelete(NULL);
}

extern void ac_state_send(void *pvParameters){
    char *eid = (char *)pvParameters;
    nvs_handle_t ir_nvs_proto_handler;
    ESP_ERROR_CHECK(nvs_open_from_partition("rogo", eid, NVS_READONLY, &ir_nvs_proto_handler));
    // int16_t eidProto = -1;
    uint16_t eidProto = -1;
    ESP_ERROR_CHECK(nvs_get_u16(ir_nvs_proto_handler, eid, &eidProto));

    // uint8_t acState[CTR_AC_FEATURE_NUM]; // Power, Mode, Temp, Fan, Swing
    struct acState acDevState;
    size_t acStateLen = sizeof(struct acState);
    ESP_ERROR_CHECK(nvs_get_blob(ir_nvs_proto_handler, "state", &acDevState, &acStateLen));

    // uint8_t power = acState[0];
    // uint8_t mode  = acState[1];
    // uint8_t temp  = acState[2];
    // uint8_t fan   = acState[3];
    // uint8_t swing = acState[4];
                
    nvs_close(ir_nvs_proto_handler);
    ESP_LOGI("IR", "Protocol: %d, Power: %d", eidProto, acDevState.power);
    ir_send_ac(eidProto, acDevState.power, acDevState.mode, acDevState.temp, acDevState.fan, acDevState.swing);
    if ((decode_type_t)eidProto == TOSHIBA_AC){
        ir_send_ac(eidProto, acDevState.power, acDevState.mode, acDevState.temp, acDevState.fan, acDevState.swing);
    }
    irSendDone = true;
    vTaskDelete(NULL);
}

extern void ac_state_send_test(void *pvParameters){
    uint8_t *controlPara = (uint8_t *)pvParameters;
    uint16_t irProto = controlPara[0] << 8 | controlPara[1];
    uint16_t power   = controlPara[4] << 8 | controlPara[5];
    uint16_t mode    = controlPara[6] << 8 | controlPara[7];
    uint16_t temp    = controlPara[8] << 8 | controlPara[9];
    uint16_t fan     = controlPara[10] << 8 | controlPara[11];
    uint16_t swing   = controlPara[12] << 8 | controlPara[13];
    while (!irSendDone){
        vTaskDelay(1);
    }
    irSendDone = false;
    ir_send_ac(irProto, power, mode, temp, fan, swing);
    irSendDone = true;
    free(controlPara);
    controlPara = NULL;
    vTaskDelete(NULL);
}