
from setuptools import setup
setup(name='iwd',
      version='1.0.0-alpha08',
      packages=['iwd'],
      entry_points={
          'console_scripts': [
              'iwd=iwd.__main__:main'
          ]
      },
      )
