
import functools

__all__ = ['cached_property']


class cached_property:
    __marker = []

    def __init__(self, method):
        functools.update_wrapper(self, method)
        self.__value = self.__marker

    def __get__(self, instance, owner):
        assert instance is not None
        if self.__value is self.__marker:
            self.__value = self.__wrapped__(instance)
        return self.__value


