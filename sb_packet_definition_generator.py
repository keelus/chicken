from enum import Enum
import os
from shutil import rmtree
from pydantic import BaseModel, RootModel

BASE_FOLDER = "./src/net/server_bound/packet_definitions"

class Category(str, Enum):
    handshake = "handshake"
    login = "login"
    play = "play"

class FieldType(str, Enum):
    varint = "varint"
    unsigned_short = "unsigned_short"
    string = "string"

class Packet(BaseModel):
    name: str
    id: int
    fields: dict[str, FieldType]

PacketDefinitions = RootModel[dict[Category, list[Packet]]]

PARSED_FIELDS: dict[FieldType, str] = {
    FieldType.varint: "usize",
    FieldType.unsigned_short:"char *",
    FieldType.string:"u16"
}

def generate_file(category: str, packet: Packet) -> str:
    field_imports = "\n".join([
        f"#include \"../../fields/{field_type}.h\""
        for field_type in packet.fields.values()
    ])
    struct_fields = "\n\t".join([
        f"{PARSED_FIELDS[field_type]} {field_name};"
        for field_name, field_type in packet.fields.items()
    ])
    field_parses = "\n\t".join([
        f"{PARSED_FIELDS[field_type]} {field_name} = net_sb_{field_type}_parse(raw_packet, &index);"
        for field_name, field_type in packet.fields.items()
    ])
    field_assignments = "\n\t".join([
        f"packet->{field_name} = {field_name};"
        for field_name in packet.fields.keys()
    ])

    include_guard = f"CHICKEN_NET_SB_PACKETS_{category.upper()}_{packet.name.upper()}"
    struct_name = f"net_sb_packets_{category}_{packet.name}"

    return f"""#ifndef {include_guard}
#define {include_guard}
// clang-format off

#include "../../../../common.h"

{field_imports}

typedef struct {{
	{struct_fields}
}} net_sb_packets_handshake_handshake;

{struct_name} * {struct_name}_parse(raw_packet_t *raw_packet) {{
	usize index = 0;

	{field_parses}

	{struct_name} *packet =
		({struct_name} *)malloc(
			sizeof({struct_name}));

	{field_assignments}
	return packet;
}}

// clang-format on
#endif"""

def main() -> None:
    with open("sb_packet_definitions.json") as f:
        json_data: str = f.read()

    try:
        definitions = PacketDefinitions.model_validate_json(json_data)
    except Exception as e:
        print(f"Error parsing the server bound packet definitions JSON: {e}")
        exit(1)

    rmtree(BASE_FOLDER)
    os.mkdir(BASE_FOLDER)
    for category, packets in definitions.root.items():
        target_folder = f"{BASE_FOLDER}/{category}"
        os.mkdir(target_folder)
        print(f"Category \"{category}\"({len(packets)}):")
        for packet in packets:
            with open(f"{target_folder}/{packet.name}.h", "w+") as f:
                _ = f.write(generate_file(category, packet))
            print(f" - Generated \"{packet.name}.h\"")


if __name__ == "__main__":
    main()
