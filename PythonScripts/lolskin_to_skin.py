import configparser
import json
import os


def get_lolskin_settings(path: str) -> dict[str, str]:
    config = configparser.ConfigParser()
    config.read(path, encoding="utf-8")

    lolskin_hero_skin = dict(config.items("SKIN_CHAMPION_ACTIVED"))
    del lolskin_hero_skin['custom_file']

    return lolskin_hero_skin


def convert_settings(lolskin_hero_skin: dict) -> dict[str, int]:
    # Convert lolskin settings to R3nzSkin settings
    r3nz_settings = {}

    for key, value in lolskin_hero_skin.items():
        item = f"{key.capitalize()}.current_combo_skin_index"
        r3nz_settings[item] = int(value) + 1

    return r3nz_settings


def main() -> None:
    while True:
        LOLSKIN_CONFIG_PATH = input("Enter the path to Lolskin Config.ini: ")
        if os.path.exists(LOLSKIN_CONFIG_PATH):
            break
        print("Lolskin Config.ini not found. Please enter a valid path.")

    lolskin_settings = get_lolskin_settings(LOLSKIN_CONFIG_PATH)
    r3nz_settings = convert_settings(lolskin_settings)

    print("Converted settings:", r3nz_settings)

    with open('data.json', 'w', encoding="utf-8") as fp:
        json.dump(r3nz_settings, fp)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        exit()
