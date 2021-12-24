from machine import I2C


# TEST CONNECTIONS
def generate_report(append_df):
    return append_df

def generate_error_report(append_df):
    return append_df

def init_i2c():

    # Create I2C Controller 
    # Find all Connections
    # Test All 
    # IF FAIL Log it
    xSCL = 'PH7'
    xSDA = 'PH8'
    xFreq = 100000

    try: 
        i2c_controller = I2C(scl= xSCL, sda=xSDA, freq=xFreq)
    except:
        print("An Error Has Occured")
    finally:
        print("Fatal Error")
    
def init_uart():

def init_adc():

def init_ble():

def init_connections():
