import test_user


if __name__ == '__main__':
    res = test_user.user_all()
    print(*res, sep='\n')