DROP TABLE IF EXISTS test cascade;

CREATE TABLE test (
	t_id INT PRIMARY KEY NOT NULL,
	t_category VARCHAR(1000),
	t_description VARCHAR(1000)
);

# Want to create specific tables for testing here such as for cache statistics, CPU cycles, etc
CREATE TABLE sys (
	sys_counting_domain varchar(20),
	sys_cores INT,
	sys_ram INT,
	sys_family varchar(50),
	sys_clock FLOAT, 
	sys_page_size SHORT,
	sys_time FLOAT,
	sys_bandwith_used_l1 FLOAT,
	sys_bandwith_used_l2 FLOAT
)

CREATE TABLE cache (
	c_level SHORT,
	c_type VARCHAR(20)
	c_size INT,
	c_line SHORT,
	c_used_ratio FLOAT,
	c_access_count INT,
	c_hit_count INT,
	c_miss_count INT,
# May want to split into data and instruction hit/miss ratios?
	c_hit_ratio FLOAT,
	c_miss_ratio FLOAT,
	c_tlb_misses INT
);

CREATE TABLE cpu (
	cpu_utilization FLOAT,
	cpu_percent_cycles_stalled_on_any_resource FLOAT,
	cpu_time FLOAT,
	cpu_instructions_per_sec_cycles FLOAT,
	cpu_instructions_per_sec_wall_clock FLOAT
);