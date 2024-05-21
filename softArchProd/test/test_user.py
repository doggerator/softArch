import requests
import json
from base64 import b64encode

BASE_USER_SERVICE_URL = 'http://localhost:8080'

def insert_user(user):
    res = requests.post(f'{BASE_USER_SERVICE_URL}/api/users', data={
        'first_name':   user['firstName'],
        'last_name':    user['lastName'],
        'email':        user['email'],
        'title':        user['title'],
        'login':        user['login'],
        'password':     user['password']
    })

    res.raise_for_status()
    return res.json()

def user_self(login, password):
    credentials = b64encode(f'{login}:{password}'.encode()).decode()
    res = requests.get(f'{BASE_USER_SERVICE_URL}/api/users/self', headers={
        'Authorization': f'Basic {credentials}'
    })
    res.raise_for_status()
    return res.json()

def user_search(first_name, last_name):
    print(f'Searching for first_name={first_name} last_name={last_name}')
    res = requests.get(f'{BASE_USER_SERVICE_URL}/api/users/search', params={
        'first_name': first_name,
        'last_name': last_name
    })
    res.raise_for_status()
    payload = res.json()
    print(*payload, sep='\n')
    return payload

def user_by_id(id):
    res = requests.get(f'{BASE_USER_SERVICE_URL}/api/users', params={ 'id': id })
    res.raise_for_status()
    return res.json()

def user_all():
    res = requests.get(f'{BASE_USER_SERVICE_URL}/api/users/all')
    res.raise_for_status()
    return res.json()

def get_test_data():
    with open('test/data.json') as file:
        return json.load(file)

def insert_test_data():
    data = get_test_data()
    users = data['User']
    for user in users:
        insert_user(user)

def test_user_by_id():
    print('test_user_by_id')

    users = user_all()

    for user in users:
        candidate = user_by_id(user['id'])
        assert candidate['id'] == user['id']
        assert candidate['login'] == user['login']
        print(candidate)

def test_auth():
    data = get_test_data()
    users = data['User']

    for user in users:
        l, p = user['login'], user['password']
        res = user_self(l, p)
        assert res['login'] == l
        print(res)

    try:
        user = user_self('login-2', 'pass-3')
        print('Auth test failed')
    except:
        pass

def test_search():
    print('test_search')
    MUST_USER_MATHCHED_NUMBER = 3
    users = user_search('user', 'last')
    print('Matched number', len(users), MUST_USER_MATHCHED_NUMBER)
    assert len(users) == MUST_USER_MATHCHED_NUMBER

    users = user_search('admin', 'admin')
    assert len(users) == 1

    users = user_search('not exists', 'last')
    assert len(users) == 0

if __name__ == '__main__':
    # insert_test_data()

    test_auth()
    # test_search()
    test_user_by_id()
