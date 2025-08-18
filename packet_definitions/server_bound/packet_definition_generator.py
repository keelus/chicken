from enum import Enum
import os
from shutil import rmtree
import sys
from pydantic import BaseModel, RootModel

BASE_FOLDER = "./src/net/server_bound/packet_definitions"

class Category(str, Enum):
    handshake = "handshake"
    login = "login"
    play = "play"

class Field(BaseModel):
    name: str
    type: "FieldType"

class FieldType(str, Enum):
    varint = "varint"
    unsigned_short = "unsigned_short"
    string = "string"

fields = [FieldType.varint, FieldType.unsigned_short, FieldType.string]

class Packet(BaseModel):
    name: str
    id: int
    fields: list[Field]

PacketDefinitions = RootModel[dict[Category, list[Packet]]]

PARSED_FIELDS: dict[FieldType, str] = {
    FieldType.varint: "usize",
    FieldType.string:"char *",
    FieldType.unsigned_short:"u16"
}

def generate_packet_declaration(category: str, packet: Packet) -> str:
    struct_fields = "\n\t".join([
        f"{PARSED_FIELDS[field.type]} {field.name};"
        for field in packet.fields
    ])

    struct_name = f"net_sb_packets_{category}_{packet.name}"

    return f"""typedef struct {{
	{struct_fields}
}} {struct_name};
{struct_name} * {struct_name}_parse(net_sb_raw_packet_t *raw_packet);
"""

def generate_header_file(category: str, packets: list[Packet]) -> str:
    field_imports = "\n".join([
        f"#include \"../fields/{field.value}.h\""
        for field in fields
    ])

    s = ""

    for packet in packets:
        s += generate_packet_declaration(category, packet)

    include_guard = f"CHICKEN_NET_SB_PACKETS_{category.upper()}"

    return f"""#ifndef {include_guard}
#define {include_guard}
// clang-format off

{field_imports}

#include "../../common/raw_packet.h"

{s}
#endif"""

def generate_packet_implementation(category: str, packet: Packet) -> str:
    field_parses = "\n\t".join([
        f"{PARSED_FIELDS[field.type]} {field.name} = net_sb_{field.type.value}_parse(raw_packet->buffer, raw_packet->len, &index);"
        for field in packet.fields
    ])
    field_assignments = "\n\t".join([
        f"packet->{field.name} = {field.name};"
        for field in packet.fields
    ])
    struct_name = f"net_sb_packets_{category}_{packet.name}"

    return f"""{struct_name} * {struct_name}_parse(net_sb_raw_packet_t *raw_packet) {{
	usize index = 0;

	{field_parses}

	{struct_name} *packet =
		({struct_name} *)malloc(
			sizeof({struct_name}));

	{field_assignments}
	return packet;
}}
"""


def generate_implementation_file(category: str, packets: list[Packet]) -> str:
    header_file_name = f"{category}.h"
    s = f"#include \"{header_file_name}\"\n\n"
    for packet in packets:
        s += generate_packet_implementation(category, packet)

    return s
        

def main() -> None:
    with open("./packet_definitions/server_bound/packet_definitions.json") as f:
        json_data: str = f.read()

    try:
        definitions = PacketDefinitions.model_validate_json(json_data)
    except Exception as e:
        print(f"Error parsing the server bound packet definitions JSON: {e}")
        exit(1)

    if os.path.exists(BASE_FOLDER):
        rmtree(BASE_FOLDER)

    print("Generating server bound packet files.")

    os.mkdir(BASE_FOLDER)
    for category, packets in definitions.root.items():
        print(f" - Category \"{category.value}\"({len(packets)}).")
        with open(f"{BASE_FOLDER}/{category.value}.h", "w+") as f:
            _ = f.write(generate_header_file(category.value, packets))
        with open(f"{BASE_FOLDER}/{category.value}.c", "w+") as f:
            _ = f.write(generate_implementation_file(category.value, packets))


if __name__ == "__main__":
    main()
