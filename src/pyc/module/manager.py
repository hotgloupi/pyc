"""Module manager class
"""
import os
import logging

from .. import source
from .. import core

from .module import Module, InternalModule

__all__ = ['Manager']

LOG = logging.getLogger(__name__)


class Manager:
    """A module manager is responsible for loading python modules from file
    or from name. It also provides basic caching so that modules are not loaded
    more than once.
    """

    def __init__(self, paths = None):
        self.paths = paths or []
        self.sources = source.Manager()
        self.cache = {}
        self.internal_module = InternalModule(node=core.internal.intrinsics, manager=self)

    def load_from_file(self,
                       path : str,
                       module_name : str,
                       builtins : Module = None) -> Module:
        """Load a module from path

        Args:
            path: Path to the python file
            module_name: the name of the module
            builtins: A builtins module

        Returns: The loaded module
        """
        module = self.cache.get(module_name)
        if module is None:
            LOG.info("loading module '%s' from '%s'", module_name, path)
            self.cache[module_name] = module = Module(
                name = module_name,
                source = self.sources.load(path),
                manager = self,
                builtins = builtins
            )
        return module

    def load_absolute(self,
                      module_path : list,
                      builtins : Module = None) -> Module:
        """Load a module from absolute name

        Args:
            module_path: A list of string representing the module full name
            builtins: A builtins module

        Returns: The loaded module
        """
        name = '.'.join(module_path)
        if name == '__pyc__':
            return self.internal_module
        # XXX try cache here
        assert isinstance(module_path, list)
        candidates = [
            '/'.join(module_path) + '.py'
        ]
        for candidate in candidates:
            for dir in self.paths:
                path = os.path.join(dir, candidate)
                if os.path.isfile(path):
                    return self.load_from_file(path, name, builtins)
        raise Exception("Couldn't import '%s'" % name)

    def load_relative(self, module_name, directory):
        raise NotImplementedError()
