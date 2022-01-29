void setLed(float GyX, float GyY) {
    bool Xflat, Yflat;
    uint8_t i;
    
    Xflat = (GyX < INCL_TOLL_X && GyX > (INCL_TOLL_X * -1)); // X and Y angles respect tollerance
    Yflat = (GyY < INCL_TOLL_Y && GyY > (INCL_TOLL_Y * -1));
    if(Xflat && Yflat) { // flat
        for(i = 0; i < 4; i++) {
            ledString.setPixelColor(i, ledString.Color(255, 0, 0));
        }
    }
    else { // not flat
        if(Xflat) { // x flat
            if(GyY >= INCL_TOLL_Y) { //y too much
                ledString.setPixelColor(0, ledString.Color(0, 255, 0));
                ledString.setPixelColor(1, ledString.Color(0, 255, 0));
                ledString.setPixelColor(2, ledString.Color(0, 0, 255));
                ledString.setPixelColor(3, ledString.Color(0, 0, 255));
            }
            else { // y too less
                ledString.setPixelColor(0, ledString.Color(0, 0, 255));
                ledString.setPixelColor(1, ledString.Color(0, 0, 255));
                ledString.setPixelColor(2, ledString.Color(0, 255, 0));
                ledString.setPixelColor(3, ledString.Color(0, 255, 0));
            }
        }
        else if(Yflat) { // y flat
            if(GyX >= INCL_TOLL_X) { //x too much
                ledString.setPixelColor(0, ledString.Color(0, 0, 255));
                ledString.setPixelColor(1, ledString.Color(0, 255, 0));
                ledString.setPixelColor(2, ledString.Color(0, 255, 0));
                ledString.setPixelColor(3, ledString.Color(0, 0, 255));
            }
            else { // x too less
                ledString.setPixelColor(0, ledString.Color(0, 255, 0));
                ledString.setPixelColor(1, ledString.Color(0, 0, 255));
                ledString.setPixelColor(2, ledString.Color(0, 0, 255));
                ledString.setPixelColor(3, ledString.Color(0, 255, 0));
            }
        }
        else {
            if(GyX >= INCL_TOLL_X) { // x too much
                if(GyY >= INCL_TOLL_Y) { // y too much
                    ledString.setPixelColor(0, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(1, ledString.Color(0, 255, 0));
                    ledString.setPixelColor(2, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(3, ledString.Color(0, 0, 255));
                }
                else { // y too less
                    ledString.setPixelColor(0, ledString.Color(0, 0, 255));
                    ledString.setPixelColor(1, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(2, ledString.Color(0, 255, 0));
                    ledString.setPixelColor(3, ledString.Color(255, 0, 0));
                }
            }
            else { // x too less
                if(GyY >= INCL_TOLL_Y) { //y too much
                    ledString.setPixelColor(0, ledString.Color(0, 255, 0));
                    ledString.setPixelColor(1, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(2, ledString.Color(0, 0, 255));
                    ledString.setPixelColor(3, ledString.Color(255, 0, 0));
                }
                else { // y too less
                    ledString.setPixelColor(0, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(1, ledString.Color(0, 0, 255));
                    ledString.setPixelColor(2, ledString.Color(255, 0, 0));
                    ledString.setPixelColor(3, ledString.Color(0, 255, 0));
                }
            }
        }
    }
    ledString.show();
}