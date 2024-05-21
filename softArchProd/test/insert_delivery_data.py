import json
import test_delivery
import test_user

# test_delivery.insert_test_data(test_delivery.session)




def insert_test_data(session):
    with open('/home/led/MAI/highload/test/data.json') as file:
        data = json.load(file)

    user_mapper = test_delivery.UserMapper()
    print(user_mapper.mapped_users)

    products = data['Product']
    for product in products:
        product['ownerId'] =  user_mapper.get_user_id(product['ownerId'])
        test_delivery.insert_product(product, session)

    deliveries = data['Delivery']
    for delivery in deliveries:
        delivery['recieverId'] = user_mapper.get_user_id(delivery['recieverId'])
        delivery['senderId'] = user_mapper.get_user_id(delivery['senderId'])
        test_delivery.insert_delivery(delivery, session)

insert_test_data(test_delivery.session)