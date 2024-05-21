import requests
import json
from base64 import b64encode
import test_user
BASE_DELIVERY_SERVICE_URL = 'http://localhost:8081'


def make_auth_header(login, password):
    credentials = b64encode(f'{login}:{password}'.encode()).decode()
    return f'Basic {credentials}'

def get_user_products(login, password, session):
    res = session.get(f'{BASE_DELIVERY_SERVICE_URL}/api/users/self', headers={
        'Authorization': make_auth_header(login, password)
    })
    res.raise_for_status()
    return res.json()

def insert_product(product, session):
    res = session.post(f'{BASE_DELIVERY_SERVICE_URL}/api/products', json={
        'ownerId': product['ownerId'],
        'name': product['name']
    })

    res.raise_for_status()
    return json.loads(res.text)

def get_products(owner_id, session):
    res = session.get(f'{BASE_DELIVERY_SERVICE_URL}/api/products', params={
        'ownerId': owner_id,
    })
    res.raise_for_status()
    return res.json()

def insert_delivery(delivery, session):
    res = session.post(f'{BASE_DELIVERY_SERVICE_URL}/api/deliveries', json={
        'senderId': delivery['senderId'],
        'recieverId': delivery['recieverId'],
        'productId': delivery['productId']
    })

    res.raise_for_status()
    return res.json()

def get_deliveries(session, sender_id=None, reciever_id=None):
    res = session.get(f'{BASE_DELIVERY_SERVICE_URL}/api/deliveries', params={
        'senderId': sender_id,
        'recieverId': reciever_id,
    })
    res.raise_for_status()
    return res.json()



class UserMapper:
    '''Users mapped by unique login field'''
    def __init__(self) -> None:
        with open('/home/led/MAI/highload/test/data.json') as file:
            data = json.load(file)

        users = { user['login']: user for user in test_user.user_all() }
        self.mapped_users = { user['__id']: users[user['login']] for user in data['User'] }

    def get_user(self, internal_id):
        return self.mapped_users[internal_id]

    def get_user_id(self, internal_id):
        return self.mapped_users[internal_id]['id']


def test_correct_delivery_filter(session):
    mapper = UserMapper()

    deliveries = get_deliveries(session, mapper.get_user_id(1), mapper.get_user_id(2))
    print(deliveries)
    for d in deliveries:
        assert d['senderId'] == mapper.get_user_id(1) and d['recieverId'] == mapper.get_user_id(2)
    print('test_correct_delivery_filter - Good sender and reciever id PASSED')

    deliveries = get_deliveries(session, sender_id=mapper.get_user_id(1))
    print(deliveries)
    for d in deliveries:
        assert d['senderId'] == mapper.get_user_id(1) and d['recieverId'] == mapper.get_user_id(2)
    print('test_correct_delivery_filter - Good sender only match PASSED')

    deliveries = get_deliveries(session, reciever_id=mapper.get_user_id(1))
    for d in deliveries:
        assert d['recieverId'] == mapper.get_user_id(1)
    print('test_correct_delivery_filter - Good recievr only match PASSED')

def test_correct_user_products(session):
    mapper = UserMapper()
    products = get_products(mapper.get_user_id(1), session)
    print(products)
    for p in products:
        assert p['ownerId'] == mapper.get_user_id(1)


session = requests.Session()
session.headers = {
    'Authorization': make_auth_header('admin', 'admin')
}

if __name__ == '__main__':
    test_correct_delivery_filter(session)
    test_correct_user_products(session)
