--
-- PostgreSQL database dump
--

-- Dumped from database version 11.5
-- Dumped by pg_dump version 11.5

--
-- Name: customer; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE customer (
    c_custkey integer NOT NULL,
    c_name character varying(25) NOT NULL,
    c_address character varying(40) NOT NULL,
    c_nationkey integer NOT NULL,
    c_phone character(15) NOT NULL,
    c_acctbal numeric(15,2) NOT NULL,
    c_mktsegment character(10) NOT NULL,
    c_comment character varying(117) NOT NULL
);



--
-- Name: lineitem; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE lineitem (
    l_orderkey integer NOT NULL,
    l_partkey integer NOT NULL,
    l_suppkey integer NOT NULL,
    l_linenumber integer NOT NULL,
    l_quantity numeric(15,2) NOT NULL,
    l_extendedprice numeric(15,2) NOT NULL,
    l_discount numeric(15,2) NOT NULL,
    l_tax numeric(15,2) NOT NULL,
    l_returnflag character(1) NOT NULL,
    l_linestatus character(1) NOT NULL,
    l_shipdate date NOT NULL,
    l_commitdate date NOT NULL,
    l_receiptdate date NOT NULL,
    l_shipinstruct character(25) NOT NULL,
    l_shipmode character(10) NOT NULL,
    l_comment character varying(44) NOT NULL
);



--
-- Name: nation; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE nation (
    n_nationkey integer NOT NULL,
    n_name character(25) NOT NULL,
    n_regionkey integer NOT NULL,
    n_comment character varying(152)
);


--
-- Name: orders; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE orders (
    o_orderkey integer NOT NULL,
    o_custkey integer NOT NULL,
    o_orderstatus character(1) NOT NULL,
    o_totalprice numeric(15,2) NOT NULL,
    o_orderdate date NOT NULL,
    o_orderpriority character(15) NOT NULL,
    o_clerk character(15) NOT NULL,
    o_shippriority integer NOT NULL,
    o_comment character varying(79) NOT NULL
);


--
-- Name: part; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE part (
    p_partkey integer NOT NULL,
    p_name character varying(55) NOT NULL,
    p_mfgr character(25) NOT NULL,
    p_brand character(10) NOT NULL,
    p_type character varying(25) NOT NULL,
    p_size integer NOT NULL,
    p_container character(10) NOT NULL,
    p_retailprice numeric(15,2) NOT NULL,
    p_comment character varying(23) NOT NULL
);



--
-- Name: partsupp; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE partsupp (
    ps_partkey integer NOT NULL,
    ps_suppkey integer NOT NULL,
    ps_availqty integer NOT NULL,
    ps_supplycost numeric(15,2) NOT NULL,
    ps_comment character varying(199) NOT NULL
);



--
-- Name: region; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE region (
    r_regionkey integer NOT NULL,
    r_name character(25) NOT NULL,
    r_comment character varying(152)
);



--
-- Name: supplier; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE supplier (
    s_suppkey integer NOT NULL,
    s_name character(25) NOT NULL,
    s_address character varying(40) NOT NULL,
    s_nationkey integer NOT NULL,
    s_phone character(15) NOT NULL,
    s_acctbal numeric(15,2) NOT NULL,
    s_comment character varying(101) NOT NULL
);



--
-- Name: customer customer_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY customer
    ADD CONSTRAINT customer_pkey PRIMARY KEY (c_custkey);


--
-- Name: lineitem lineitem_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY lineitem
    ADD CONSTRAINT lineitem_pkey PRIMARY KEY (l_orderkey, l_linenumber);


--
-- Name: nation nation_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY nation
    ADD CONSTRAINT nation_pkey PRIMARY KEY (n_nationkey);


--
-- Name: orders orders_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY orders
    ADD CONSTRAINT orders_pkey PRIMARY KEY (o_orderkey);


--
-- Name: part part_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY part
    ADD CONSTRAINT part_pkey PRIMARY KEY (p_partkey);


--
-- Name: partsupp partsupp_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY partsupp
    ADD CONSTRAINT partsupp_pkey PRIMARY KEY (ps_partkey, ps_suppkey);


--
-- Name: region region_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY region
    ADD CONSTRAINT region_pkey PRIMARY KEY (r_regionkey);


--
-- Name: supplier supplier_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY supplier
    ADD CONSTRAINT supplier_pkey PRIMARY KEY (s_suppkey);


--
-- Name: idx_customer_nationkey; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_customer_nationkey ON customer USING btree (c_nationkey);


--
-- Name: idx_lineitem_orderkey; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_lineitem_orderkey ON lineitem USING btree (l_orderkey);


--
-- Name: idx_lineitem_part_supp; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_lineitem_part_supp ON lineitem USING btree (l_partkey, l_suppkey);


--
-- Name: idx_lineitem_shipdate; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_lineitem_shipdate ON lineitem USING btree (l_shipdate, l_discount, l_quantity);


--
-- Name: idx_nation_regionkey; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_nation_regionkey ON nation USING btree (n_regionkey);


--
-- Name: idx_orders_custkey; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_orders_custkey ON orders USING btree (o_custkey);


--
-- Name: idx_orders_orderdate; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_orders_orderdate ON orders USING btree (o_orderdate);


--
-- Name: idx_partsupp_partkey; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_partsupp_partkey ON partsupp USING btree (ps_partkey);


--
-- Name: idx_partsupp_suppkey; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_partsupp_suppkey ON partsupp USING btree (ps_suppkey);


--
-- Name: idx_supplier_nation_key; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_supplier_nation_key ON supplier USING btree (s_nationkey);


--
-- PostgreSQL database dump complete
--

