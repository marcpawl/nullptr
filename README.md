nullptr

Slideware for demonstrating safe use of nullptr
by wrappoing pointers into strong types.

Quality:  slideware, use for ideas

Building:
  Have docker installed.

  make

  Creates the docker image, launches the container and
  builds the project, and enters the container in a shell.

  Within the container, the command
    make cmake
  will build everything.

  Project is mappped into the local file system.

  Tested with wsl Ubuntu.
