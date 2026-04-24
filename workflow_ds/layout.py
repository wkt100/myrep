"""Auto-layout for Dify workflow nodes."""

from typing import Any, Dict, List, Tuple

# Layout constants
X_START = 80
X_SPACING = 260
Y_START = 280
Y_STEP = 120
ITERATION_INNER_X = 128
ITERATION_INNER_Y = 68
NODE_DEFAULT_HEIGHT = 87


def layout_workflow(
    nodes_spec: List[Dict[str, Any]],
    edges: List[Dict[str, str]],
) -> Dict[str, Tuple[float, float]]:
    """Calculate (x, y) positions for all nodes.

    Simple layered layout:
    - Main chain nodes flow left to right.
    - Branch nodes (from if-else false port) shift down by Y_STEP.
    - Iteration container is placed as a regular node; its children
      are positioned relative to the container.

    Returns dict of node_id -> (x, y).
    """
    positions: Dict[str, Tuple[float, float]] = {}

    # Build adjacency: node -> list of (target, handle)
    from_map: Dict[str, List[Tuple[str, str]]] = {}
    to_map: Dict[str, List[Tuple[str, str]]] = {}
    for e in edges:
        s = e["from"]
        t = e["to"]
        h = e.get("handle", "source")
        from_map.setdefault(s, []).append((t, h))
        to_map.setdefault(t, []).append((s, h))

    # Find root (node with no incoming edges)
    all_nodes = {n["id"] for n in nodes_spec}
    has_incoming = set()
    for e in edges:
        has_incoming.add(e["to"])
    roots = all_nodes - has_incoming
    if not roots:
        # fallback: first node in spec
        roots = {nodes_spec[0]["id"] if nodes_spec else ""}

    # Track branching offsets
    branch_offsets: Dict[str, int] = {}  # node_id -> additional y offset

    # BFS to assign positions
    visited: set = set()
    queue: List[Tuple[str, int, int]] = [(r, 0, 0) for r in roots]  # (node_id, depth, branch_offset)
    idx = 0
    while idx < len(queue):
        nid, depth, boff = queue[idx]
        idx += 1
        if nid in visited:
            continue
        visited.add(nid)

        # Check if this node is inside an iteration
        node_spec = next((n for n in nodes_spec if n["id"] == nid), None)
        is_in_iteration = False
        if node_spec and "iteration_id" in node_spec.get("config", {}):
            is_in_iteration = True

        if not is_in_iteration:
            positions[nid] = (X_START + depth * X_SPACING, Y_START + boff * Y_STEP)
        else:
            # Position will be relative to parent iteration container
            # handled separately
            pass

        children = from_map.get(nid, [])
        for i, (child, handle) in enumerate(children):
            if child in visited:
                continue
            new_boff = boff
            if handle == "false":
                new_boff = boff + 1
            queue.append((child, depth + 1, new_boff))

    return positions


def assign_node_heights(spec_nodes: List[Dict[str, Any]]) -> Dict[str, float]:
    """Assign heights based on node type."""
    heights: Dict[str, float] = {}
    for n in spec_nodes:
        ntype = n.get("type", "")
        if ntype in ("start", "end"):
            heights[n["id"]] = 88.0
        elif ntype == "iteration":
            heights[n["id"]] = 176.0
        elif ntype == "if-else":
            heights[n["id"]] = 148.0
        elif ntype == "agent":
            heights[n["id"]] = 187.0
        else:
            heights[n["id"]] = 87.0
    return heights
