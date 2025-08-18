from enum import Enum
import os
from pathlib import Path
from shutil import rmtree
import sys
from pydantic import BaseModel, RootModel

BASE_FOLDER = "./src/net/client_bound/packet_definitions"

class Category(str, Enum):
    handshake = "handshake"
    login = "login"
    play = "play"

class Field(BaseModel):
    name: str
    type: "FieldType"

class FieldType(str, Enum):
    bool = "bool"
    byte = "byte"
    short = "short"
    int = "int"
    long = "long"
    int_128 = "int_128"
    float = "float"
    double = "double"
    string = "string"
    varint = "varint"
    raw_bytes = "raw_bytes"
    unsigned_byte = "unsigned_byte"
    unsigned_short = "unsigned_short"


class Packet(BaseModel):
    name: str
    id: int
    fields: list[Field]

PacketDefinitions = RootModel[dict[Category, list[Packet]]]

PRIMITIVE_FIELDS: dict[FieldType, list[str]] = {
    FieldType.bool: ["bool"],
    FieldType.byte: ["i8"],
    FieldType.short: ["i16"],
    FieldType.int: ["i32"],
    FieldType.long: ["i64"],
    FieldType.int_128: ["i64", "i64"],
    FieldType.float: ["float"],
    FieldType.double: ["double"],
    FieldType.string: ["char *"],
    FieldType.varint: ["u32"],
    FieldType.raw_bytes: ["u8 *", "usize"],
    FieldType.unsigned_byte: ["u8"],
    FieldType.unsigned_short: ["u16"],
}

def generate_function_arguments(packet: Packet) -> str:
    fields: list[str] = []
    for field in packet.fields:
        primitive_fields = PRIMITIVE_FIELDS[field.type]
        if len(primitive_fields) == 1:
            fields.append(f"{primitive_fields[0]} {field.name}")
        else:
            for i, primitive_field in enumerate(primitive_fields):
                fields.append(f"{primitive_field} {field.name}_{i}")

    return "\n\t\t" + ",\n\t\t".join(fields)

def generate_packet_declaration(category: str, packet: Packet) -> str:
    function_name = f"net_cb_packets_{category}_{packet.name}_create"
    function_arguments = generate_function_arguments(packet)

    return f"net_cb_packet* {function_name}({function_arguments});\n"

def generate_header_file(category: str, packets: list[Packet]) -> str:
    s = ""

    for packet in packets:
        s += generate_packet_declaration(category, packet)

    include_guard = f"CHICKEN_NET_CB_PACKETS_{category.upper()}"

    return f"""#ifndef {include_guard}
#define {include_guard}
// clang-format off

#include "../packet_builder/packet_builder.h"

{s}
#endif"""

def generate_packet_implementation(category: str, packet: Packet) -> str:
    function_name = f"net_cb_packets_{category}_{packet.name}_create"
    function_arguments = generate_function_arguments(packet)

    field_assignments = f"\tnet_cb_packet_builder_add_varint(packet, {packet.id});\n"
    for field in packet.fields:
        primitive_fields = PRIMITIVE_FIELDS[field.type]

        if len(primitive_fields) == 1:
            field_assignments += f"\tnet_cb_packet_builder_add_{field.type.value}(packet, {field.name});\n"
        else:
            arguments = ", ".join([
                f"{field.name}_{i}"
                for i in range(len(primitive_fields))
            ])

            field_assignments += f"\tnet_cb_packet_builder_add_{field.type.value}(packet, {arguments});\n"

    return f"""
net_cb_packet* {function_name}({function_arguments}) {{
	net_cb_packet *packet = (net_cb_packet *)malloc(sizeof(net_cb_packet));
	packet->buffer = NULL;
	packet->len = 0;

{field_assignments}
	return packet;
}}
"""

def generate_implementation_file(category: str, packets: list[Packet]) -> str:
    header_file_name = f"{category}.h"
    s = f"""#include \"{header_file_name}\"
// clang-format off
"""
    for packet in packets:
        s += generate_packet_implementation(category, packet)

    return s
        

def main() -> None:
    with open("./packet_definitions/client_bound/packet_definitions.json") as f:
        json_data: str = f.read()

    try:
        definitions = PacketDefinitions.model_validate_json(json_data)
    except Exception as e:
        print(f"Error parsing the server bound packet definitions JSON: {e}")
        exit(1)

    if os.path.exists(BASE_FOLDER):
        rmtree(BASE_FOLDER)

    print("Generating client bound packet files.")

    os.mkdir(BASE_FOLDER)
    for category, packets in definitions.root.items():
        print(f" - Category \"{category.value}\"({len(packets)}).")
        with open(f"{BASE_FOLDER}/{category.value}.h", "w+") as f:
            _ = f.write(generate_header_file(category.value, packets))
        with open(f"{BASE_FOLDER}/{category.value}.c", "w+") as f:
            _ = f.write(generate_implementation_file(category.value, packets))


if __name__ == "__main__":
    main()
