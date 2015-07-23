import requests


def get_backers_list(team_id):

    url = 'https://www.bountysource.com/backers'

    params = {'order': '+amount',
              'per_page': '1000',
              'team_id': team_id}

    headers = {
        'Accept': 'application/vnd.bountysource+json; version=2',
        'Referer': 'https://www.bountysource.com/teams'
    }

    r = requests.get(url, headers=headers, params=params)
    return r.json()


data = get_backers_list(243)

print('## Redis Desktop Manager Backers ##', '\n')

counter = 1
for backer in data:
    print('%s. ' % counter, backer['display_name'])
    counter += 1
