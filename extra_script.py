Import("env")
env.Replace(
    LDSCRIPT_PATH="./boards/stm32_rom.ld"
)