from updater import Updater, SWDLBlock

if __name__ == "__main__":
    upd = Updater()
    upd.init_update("127.0.0.1", SWDLBlock.MCU_APPLICATION, "ECO_MainMCU_App_signed.bin")