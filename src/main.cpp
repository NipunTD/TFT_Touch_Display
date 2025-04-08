#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include <lv_conf.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include "string.h"


#define  GFX_BL 38
#define  SDA_PIN 1                                                // GPIO1 as SDA
#define  SCL_PIN 2                                                // GPIO2 as SCL



String temp = "S-Err";
String rh   = "S-Err";


static lv_disp_draw_buf_t   draw_buf;
static lv_color_t          *disp_draw_buf;
static lv_color_t          *disp_draw_buf1;
static lv_disp_drv_t        disp_drv;
static lv_obj_t            *tabview;
static lv_obj_t            *main_container;
static lv_obj_t            *view1;
static lv_obj_t            *view2;
static lv_obj_t            *view3;
static lv_obj_t            *label2;
static lv_obj_t            *label4;
static lv_obj_t            *section2;
static lv_obj_t            *section4;

unsigned long lastSensorRead = 0;
const unsigned long sensorReadInterval = 2000; // Read sensor every 2 seconds
Adafruit_SHT31 sht31 = Adafruit_SHT31();


Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
    39 /* CS */, 48 /* SCK */, 47 /* SDA */, 
    18 /* DE */, 17 /* VSYNC */, 16 /* HSYNC */, 21 /* PCLK */, 
    11 /* R0 */, 12 /* R1 */, 13 /* R2 */, 14 /* R3 */, 0  /* R4 */,
    8  /* G0 */, 20 /* G1 */, 3  /* G2 */, 46 /* G3 */, 9  /* G4 */, 10 /* G5 */,
    4  /* B0 */, 5  /* B1 */, 6  /* B2 */, 7  /* B3 */, 15 /* B4 */);
 
Arduino_ST7701_RGBPanel *gfx = new Arduino_ST7701_RGBPanel(
    bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation */,
    true /* IPS */, 480 /* width */, 480 /* height */,
    st7701_type1_init_operations, sizeof(st7701_type1_init_operations), true /* BGR */,
    10 /* hsync_front_porch */, 8 /* hsync_pulse_width */, 50 /* hsync_back_porch */,
    10 /* vsync_front_porch */, 8 /* vsync_pulse_width */, 20 /* vsync_back_porch */);


void updateSensorData() {
    if (sht31.begin(0x44)) {
        float t = sht31.readTemperature();
        float h = sht31.readHumidity();
        Serial.println(t);
        Serial.println(h);

        if (!isnan(t)) {
            temp = String(t, 1) + " °C";
        } else {
            temp = "S-Err";
        }

        if (!isnan(h)) {
            rh = String(h, 0) + "%";
        } else {
            rh = "S-Err";
        }
    } else {
        temp = "S-Err";
        rh = "S-Err";
    }

    // Update the labels
    lv_label_set_text(label2, temp.c_str());
    lv_label_set_text(label4, rh.c_str());
}


    void createLayout() {
        // Create main container
        main_container = lv_obj_create(lv_scr_act());
        lv_obj_set_size(main_container, 480, 480);
        lv_obj_set_flex_flow(main_container, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_bg_color(main_container, lv_color_hex(0x000000), 0);            // Black background

    
        // Create first view (2/3 of screen height)
        view1 = lv_obj_create(main_container);
        lv_obj_set_size(view1, 445, 130);                                                // Set the size of view1
        lv_obj_set_style_bg_color(view1, lv_color_hex(0x000000), 0);                     // Black background
        lv_obj_set_style_pad_all(view1, 0, 0);

        // Create a container inside view1 for two sections
        lv_obj_t *view1_container = lv_obj_create(view1);
        lv_obj_set_size(view1_container, LV_PCT(100), LV_PCT(100));                      // Full size of view1
        lv_obj_set_flex_flow(view1_container, LV_FLEX_FLOW_ROW);                         // Arrange items in a row
        lv_obj_set_flex_align(view1_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_bg_color(view1_container, lv_color_hex(0x000000), 0);
        lv_obj_set_style_border_width(view1_container, 0, 0);                            // Remove border

        // Create first section (left side)
        lv_obj_t *section1 = lv_obj_create(view1_container);
        lv_obj_set_size(section1, LV_PCT(50), LV_PCT(100));                              // Half of view1
        lv_obj_set_style_bg_color(section1, lv_color_hex(0x000000), 0);
        lv_obj_set_style_border_width(section1, 0, 0);                                   // No border

        // Create a label inside section1
        lv_obj_t *label1 = lv_label_create(section1);
        lv_label_set_text(label1, "Temp");

        // Apply bold font style
        static lv_style_t           style;
        lv_style_init(&style);
        lv_style_set_text_font(&style, &lv_font_montserrat_48);                          // Large bold font
        lv_style_set_text_color(&style, lv_color_hex(0xffffff));                         // White

        lv_obj_add_style(label1, &style, 0);
        lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);

        // Create second section (right side)
        lv_obj_t *section2 = lv_obj_create(view1_container);
        lv_obj_set_size(section2, LV_PCT(50), LV_PCT(100));                              // Other half
        lv_obj_set_style_bg_color(section2, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_border_width(section2, 0, 0);

        // Create a label inside section2
        label2 = lv_label_create(section2);
        const char* temp_cstr = temp.c_str();
        lv_label_set_text(label2, temp_cstr);                                             // Change this to any text

        static lv_style_t  style2;
        lv_style_init(&style2);
        lv_style_set_text_font(&style2, &lv_font_montserrat_48);                        // Large bold font
        lv_style_set_text_color(&style2, lv_color_hex(0x000000));                       // White

        lv_obj_add_style(label2, &style2, 0);
        lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);


        // Create first view (2/3 of screen height)
        view2 = lv_obj_create(main_container);
        lv_obj_set_size(view2, 445, 130);                                               // Set the size of view1
        lv_obj_set_style_bg_color(view2, lv_color_hex(0x000000), 0);                    // Black background
        lv_obj_set_style_pad_all(view2, 0, 0);

        // Create a container inside view1 for two sections
        lv_obj_t *view2_container = lv_obj_create(view2);
        lv_obj_set_size(view2_container, LV_PCT(100), LV_PCT(100));                     // Full size of view1
        lv_obj_set_flex_flow(view2_container, LV_FLEX_FLOW_ROW);                        // Arrange items in a row
        lv_obj_set_flex_align(view2_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_bg_color(view2_container, lv_color_hex(0x000000), 0);
        lv_obj_set_style_border_width(view2_container, 0, 0);                           // Remove border

        // Create first section (left side)
        lv_obj_t *section3 = lv_obj_create(view2_container);
        lv_obj_set_size(section3, LV_PCT(50), LV_PCT(100));                             // Half of view1
        lv_obj_set_style_bg_color(section3, lv_color_hex(0x000000), 0);
        lv_obj_set_style_border_width(section3, 0, 0);                                  // No border

        // Create a label inside section1
        lv_obj_t *label3 = lv_label_create(section3);
        lv_label_set_text(label3, "RH");

        // Apply bold font style
        static lv_style_t  style3;
        lv_style_init(&style3);
        lv_style_set_text_font(&style3, &lv_font_montserrat_48);                        // Large bold font
        lv_style_set_text_color(&style3, lv_color_hex(0xffffff));                       // Dark yellow

        lv_obj_add_style(label3, &style3, 0);
        lv_obj_align(label3, LV_ALIGN_CENTER, 0, 0);

        // Create second section (right side)
        lv_obj_t *section4 = lv_obj_create(view2_container);
        lv_obj_set_size(section4, LV_PCT(50), LV_PCT(100));                             // Other half
        lv_obj_set_style_bg_color(section4, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_border_width(section4, 0, 0);

        // Create a label inside section2
        label4 = lv_label_create(section4);
        const char* rh_cstr = rh.c_str();
        lv_label_set_text(label4,rh_cstr);                                              // Change this to any text

        static lv_style_t  style4;
        lv_style_init(&style4);
        lv_style_set_text_font(&style4, &lv_font_montserrat_48);                        // Large bold font
        lv_style_set_text_color(&style4, lv_color_hex(0x000000));                       // Dark yellow

        lv_obj_add_style(label4, &style4, 0);
        lv_obj_align(label4, LV_ALIGN_CENTER, 0, 0);


        // Create second view (1/3 of screen height)
        view3 = lv_obj_create(main_container);
        lv_obj_set_size(view3, 445, 170);                                                                   // 1/3 of 480
        lv_obj_set_style_bg_color(view3, lv_color_hex(0x000000), 0);                                        // Black background
        lv_obj_set_style_pad_all(view3, 0, 0);
        lv_obj_set_flex_flow(view3, LV_FLEX_FLOW_COLUMN);                                                   // Arrange children vertically
        lv_obj_set_flex_align(view3, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        // Create row1 (1/3 of view3 height)
        lv_obj_t *row1 = lv_obj_create(view3);
        lv_obj_set_size(row1, LV_PCT(100), LV_PCT(33));                                                     // 1/3 of view3
        lv_obj_set_style_bg_color(row1, lv_color_hex(0x000000), 0);                                         // Black background
        lv_obj_set_style_border_width(row1, 0, 0);

        // Create "Location" label in row1
        lv_obj_t *label6 = lv_label_create(row1);
        lv_label_set_text(label6, "Location");

        // Style for white text
        static lv_style_t white_text_style;
        lv_style_init(&white_text_style);
        lv_style_set_text_font(&white_text_style, &lv_font_montserrat_48);
        lv_style_set_text_color(&white_text_style, lv_color_hex(0xffffff));                                  // White

        lv_obj_add_style(label6, &white_text_style, 0);
        lv_obj_align(label6, LV_ALIGN_CENTER, 0, 0);

        // Create row2 (2/3 of view3 height)
        lv_obj_t *row2 = lv_obj_create(view3);
        lv_obj_set_size(row2, LV_PCT(90), LV_PCT(52));                                                      // 2/3 of view3
        lv_obj_set_style_bg_color(row2, lv_color_hex(0xffffff), 0);                                         // White background
        lv_obj_set_style_border_width(row2, 0, 0);

        // Create "Area 01" label in row2
        lv_obj_t *label5 = lv_label_create(row2);
        lv_label_set_text(label5, "FG Stores");

        // Style for black text
        static lv_style_t black_text_style;
        lv_style_init(&black_text_style);
        lv_style_set_text_font(&black_text_style, &lv_font_montserrat_48);
        lv_style_set_text_color(&black_text_style, lv_color_hex(0x000000));                                 // Black

        lv_obj_add_style(label5, &black_text_style, 0);
        lv_obj_align(label5, LV_ALIGN_CENTER, 0, 0);

    }
    
    

    void setup() {
        Serial.begin(115200);
       
        Wire.begin(SDA_PIN, SCL_PIN);
        
        if (sht31.begin(0x44)) {
            temp = String(sht31.readTemperature(), 1) + "°C";
            rh = String(sht31.readHumidity(), 0) + "%";
        }
        
        Serial.print(temp);
        
        gfx->begin(12000000);
    
        // Set screen rotation (Change value to 1, 2, or 3 as needed)
        gfx->setRotation(2);  // Rotates the screen 90 degrees
    
        pinMode(GFX_BL, OUTPUT);      
        digitalWrite(GFX_BL, HIGH);
        
        lv_init();
    
        // Initialize display buffer
        static lv_disp_draw_buf_t draw_buf;
        static lv_color_t buf1[480 * 10];                                                     
        lv_disp_draw_buf_init(&draw_buf, buf1, NULL, 480 * 10);
    
        static lv_disp_drv_t disp_drv;
        lv_disp_drv_init(&disp_drv);
        disp_drv.hor_res = gfx->width();
        disp_drv.ver_res = gfx->height();
        disp_drv.flush_cb = [](lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p) {
            gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, 
                                    area->x2 - area->x1 + 1, area->y2 - area->y1 + 1);
            lv_disp_flush_ready(drv);
        };
        disp_drv.draw_buf = &draw_buf;
        lv_disp_drv_register(&disp_drv);
    
        lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0);
    
        createLayout();
    }
    

void loop() {

    if (millis() - lastSensorRead >= sensorReadInterval) {
        lastSensorRead = millis();
        updateSensorData();
        
    }

    lv_timer_handler();
    delay(5);
    

}
