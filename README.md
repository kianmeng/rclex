[![Hex version](https://img.shields.io/hexpm/v/rclex.svg "Hex version")](https://hex.pm/packages/rclex)
[![API docs](https://img.shields.io/hexpm/v/rclex.svg?label=hexdocs "API docs")](https://hexdocs.pm/rclex/readme.html)
[![License](https://img.shields.io/hexpm/l/rclex.svg)](https://github.com/rclex/rclex/blob/main/LICENSE)
[![ci-latest_push](https://github.com/rclex/rclex/actions/workflows/ci_latest.yml/badge.svg)](https://github.com/rclex/rclex/actions/workflows/ci_latest.yml)
[![ci-allver_PR](https://github.com/rclex/rclex/actions/workflows/ci_allver.yml/badge.svg)](https://github.com/rclex/rclex/actions/workflows/ci_allver.yml)

[日本語のREADME](README_ja.md)

# Rclex

Rclex is a ROS 2 client library for Elixir.

This library lets you perform basic ROS 2 behaviors by calling out from Elixir code into the RCL (ROS Client Library) API, which
uses the ROS 2 common hierarchy.

Additionally, publisher-subscriber (PubSub) communication between nodes and associated callback functions are executed by *tasks*,
which are part of a lightweight process model. This enables generation of and communication between a large number of fault-tolerant
nodes while suppressing memory load.

## What is ROS 2

ROS (Robot Operating System) is a next-generation Robot development platform. In both ROS and ROS 2, each functional
unit is exposed as a node, and by combining these nodes you can create different robot applications. Additionally,
communication between nodes uses a PubSub model where publisher and subscriber exchange information by specifying a
common topic name.

The biggest difference between ROS and ROS 2 is that the DDS (Data Distribution Service) protocol was adopted for
communication, and the library was divided in a hierarchical structure, allowing for the creation of ROS 2 client
libraries in various languages. This has allowed for the creation of a robot application library in Elixir.

For details on ROS 2, see the official [ROS 2 documentation](https://index.ros.org/doc/ros2/).

## Recommended environment

### The environment where host (development) and target (operation) are the same

Currently, we use the following environment as the main development target:

- Ubuntu 20.04.2 LTS (Focal Fossa)
- ROS 2 [Foxy Fitzroy](https://docs.ros.org/en/foxy/Releases/Release-Foxy-Fitzroy.html)
- Elixir 1.13.4-otp-25
- Erlang/OTP 25.0.3

For other environments used to check the operation of this library,
please refer to [here](https://github.com/rclex/rclex_docker#available-versions-docker-tags).

### Docker environment

The pre-built Docker images are available at [Docker Hub](https://hub.docker.com/r/rclex/rclex_docker).
You can also try the power of Rclex with it easily. Please check ["Docker Environment"](#Docker-environment) section for details.

### Nerves device (target)

`rclex` can be operated onto Nerves. In this case, you do not need to prepare the ROS 2 environment on the host computer to build Nerves project (so awesome!).

Please refer to [Use on Nerves](USE_ON_NERVES.md) section and [b5g-ex/rclex_on_nerves](https://github.com/b5g-ex/rclex_on_nerves) example repository for more details!

## Features

Currently, the Rclex API allows for the following:

1. The ability to create a large number of publishers sending to the same topic.
2. The ability to create large numbers of each combination of publishers, topics, and subscribers.

Documentation can be generated with [ExDoc](https://github.com/elixir-lang/ex_doc)
and published on [HexDocs](https://hexdocs.pm).  
You can find the docs at [https://hexdocs.pm/rclex](https://hexdocs.pm/rclex).

Please refer [rclex/rclex_examples](https://github.com/rclex/rclex_examples) for the examples of usage along with the sample code.

## How to use

This section explains the quickstart for `rclex` onto the environment where ROS 2 and Elixir have been installed.

### Create the project

First of all, create the Mix project as a normal Elixir project.

```
mix new rclex_usage
cd rclex_usage
```

### Install rclex

`rclex` is [available in Hex](https://hex.pm/docs/publish).

You can install this package into your project
by adding `rclex` to your list of dependencies in `mix.exs`:

```elixir
  defp deps do
    [
      ...
      {:rclex, "~> 0.8.3"},
      ...
    ]
  end
```

After that, execute `mix deps.get` into the project repository.

```
mix deps.get
```

### Setup the ROS 2 environment

```
source /opt/ros/foxy/setup.bash
```

## Configure ROS 2 message types you want to use

Rclex provides pub/sub based topic communication using the message type defined in ROS 2. Please refer [here](https://docs.ros.org/en/foxy/Concepts/About-ROS-Interfaces.html) for more details about message types in ROS 2.

The message types you want to use in your project can be specified in `ros2_message_types` in `config/config.exs`. 
Multiple message types can be specified separated by comma `,`.

The following `config/config.exs` example wants to use `String` type.

```elixir
import Config

config :rclex, ros2_message_types: ["std_msgs/msg/String"]
```

Then, execute the following Mix task to generate required definitions and files for message types.

```
mix rclex.gen.msgs
```

If you want to change the message types in config, do `mix rclex.gen.msgs` again.

### Write Rclex code

Now, you can acquire the environment for [Rclex API](https://hexdocs.pm/rclex/api-reference.html)! Of course, you can execute APIs on IEx directly.

Here is the simplest implementation example `lib/rclex_usage.ex` that will publish the string to `/chatter` topic.

```elixir
defmodule RclexUsage do
  def publish_message do
    context = Rclex.rclexinit()
    {:ok, node} = Rclex.ResourceServer.create_node(context, 'talker')
    {:ok, publisher} = Rclex.Node.create_publisher(node, 'StdMsgs.Msg.String', 'chatter')

    msg = Rclex.Msg.initialize('StdMsgs.Msg.String')
    data = "Hello World from Rclex!"
    msg_struct = %Rclex.StdMsgs.Msg.String{data: String.to_charlist(data)}
    Rclex.Msg.set(msg, msg_struct, 'StdMsgs.Msg.String')

    # This sleep is essential for now, see Issue #212
    Process.sleep(100)

    IO.puts("Rclex: Publishing: #{data}")
    Rclex.Publisher.publish([publisher], [msg])

    Rclex.Node.finish_job(publisher)
    Rclex.ResourceServer.finish_node(node)
    Rclex.shutdown(context)
  end
end
```

Please also check the examples for Rclex.
- [rclex/rclex_examples](https://github.com/rclex/rclex_examples)

### Build and Execute

```
mix compile
iex -S mix
```

Operate the following command on IEx.

```
iex()> RclexUsage.publish_message

00:04:40.701 [debug] JobExecutor start
 
00:04:40.705 [debug] talker0/chatter/pub
Rclex: Publishing: Hello World from Rclex!

00:04:40.706 [debug] publish ok
 
00:04:40.706 [debug] publisher finished: talker0/chatter/pub
 
00:04:40.710 [debug] finish node: talker0
{:ok, #Reference<0.2970499651.1284374532.3555>}
```

You can confirm the above operation by subscribing with `ros2 topic echo` from the other terminal.

```
$ source /opt/ros/foxy/setup.bash
$ ros2 topic echo /chatter std_msgs/msg/String
data: Hello World from Rclex!
---
```

## Enhance devepoment experience

This section describes the information mainly for developers.

### Docker environment

This repository provides a `docker compose` environment for library development with Docker.

As mentioned above, pre-built Docker images are available at [Docker Hub](https://hub.docker.com/r/rclex/rclex_docker), which can be used to easily try out Rclex.
You can set the environment variable `$RCLEX_DOCKER_TAG` to the version of the target environment. Please refer to [here](https://github.com/rclex/rclex_docker#available-versions-docker-tags) for the available environments.

```
# optional: set to the target environment (default `latest`)
export RCLEX_DOCKER_TAG=latest
# create and start the container
docker compose up -d
# execute the container (with the workdir where this repository is mounted)
docker compose exec -w /root/rclex rclex_docker /bin/bash
# stop the container
docker compose down
```

### Automatic execution of mix test, etc.

`mix test.watch` is introduced to automatically run unit test `mix test` and code formatting `mix format` every time the source code was editted.

```
$ mix test.watch
# or, run on docker by following
$ docker compose run --rm -w /root/rclex rclex_docker mix test.watch
```

### Confirmation of operation

To check the operation of this library, we prepare [rclex/rclex_connection_tests](https://github.com/rclex/rclex_connection_tests) to test the communication with the nodes implemented with Rclcpp.

```
cd /path/to/yours
git clone https://github.com/rclex/rclex
git clone https://github.com/rclex/rclex_connection_tests
cd /path/to/yours/rclex_connection_tests
./run-all.sh
```

In [GitHub Actions](https://github.com/rclex/rclex/actions), we perform CI on multiple environments at Pull Requests. HOwever, we cannot guarantee operation in all of these environments.

## Maintainers and developers (including past)

- [@takasehideki](https://github.com/takasehideki)
- [@HiroiImanishi](https://github.com/HiroiImanishi)
- [@kebus426](https://github.com/kebus426)
- [@shiroro466](https://github.com/shiroro466)
- [@s-hosoai](https://github.com/s-hosoai)
