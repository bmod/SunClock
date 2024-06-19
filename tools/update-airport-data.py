import csv
import logging
import os
import urllib.request

LOG = logging.getLogger(__name__)

BASE_DIR = f'{os.path.dirname(__file__)}/..'  # project root

# Airport data
AIRPORTS_URL = 'https://davidmegginson.github.io/ourairports-data/airports.csv'
AIRPORTS_DL_FILE = f'{BASE_DIR}/tmp/downloaded_airports.csv'

# iata -> timezone id map
TZMAP_URL = 'https://raw.github.com/hroptatyr/dateutils/tzmaps/iata.tzmap'
TZMAP_DL_FILE = f'{BASE_DIR}/tmp/iata.tzmap'

AIRPORTS_FILE = f'{BASE_DIR}/res/airports.csv'
KEEP_FIELDS = {'name', 'longitude_deg', 'latitude_deg', 'iso_country', 'municipality', 'iata_code'}
NEW_FIELDS = {'timezone'}


def downloadData():
    files = [
        (AIRPORTS_URL, AIRPORTS_DL_FILE),
        (TZMAP_URL, TZMAP_DL_FILE),
    ]

    for url, filePath in files:
        LOG.info(f'Downloading {url}')
        data = urllib.request.urlopen(url).read().decode()
        folder = os.path.dirname(filePath)
        if not os.path.exists(folder):
            os.makedirs(folder)
        with open(filePath, 'w', encoding='utf-8') as fp:
            fp.write(data)
        assert os.path.exists(filePath)
        LOG.info(f'Wrote: {filePath}')


def condenseAirports():
    """Read downloaded airport data and extract only the largest ones"""

    with open(TZMAP_DL_FILE, 'r', encoding='utf-8') as fp:
        lines = fp.readlines()
        cells = (n.strip().split() for n in lines)
        tzMap = {k: c for k, c in cells}

    with open(AIRPORTS_DL_FILE, 'r', encoding='utf-8') as fpr:
        reader = csv.DictReader(fpr)

        for f in KEEP_FIELDS:
            assert f in reader.fieldnames, f'Field not found in CSV: "{f}"'

        with open(AIRPORTS_FILE, 'w', encoding='utf-8') as fpw:
            writer = csv.DictWriter(fpw, fieldnames=KEEP_FIELDS | NEW_FIELDS, lineterminator='\n')
            writer.writeheader()
            for row in reader:

                if row['type'] != 'large_airport':
                    continue

                iata = row.get('iata_code')
                if not iata:
                    continue

                timeZone = tzMap.get(iata)
                if not timeZone:
                    LOG.warning(f'No timezone found for iata "{iata}", ignoring')
                    continue

                newRowData = {k: v for k, v in row.items() if k in KEEP_FIELDS}
                newRowData['timezone'] = timeZone
                writer.writerow(newRowData)


def updateAirports():
    downloadData()

    condenseAirports()


if __name__ == '__main__':
    updateAirports()
