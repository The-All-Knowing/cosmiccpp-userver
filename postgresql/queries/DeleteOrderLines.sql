DELETE FROM allocation.order_lines
WHERE id IN ($1);